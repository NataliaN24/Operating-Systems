#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

if [[ ! -f "$1" ]]; then
    exit 2
fi

if [[ $(whoami) != "root" ]]; then
    exit 3
fi

while read -r path type perm; do

    while read -r file; do

        currentPerm=$(stat -c '%a' "$file")
        currentType=$(stat -c '%F' "$file")

        if [[ "$type" == "R" ]]; then
            if [[ "$currentPerm" == "$perm" ]]; then
                if [[ "$currentType" == "regular file" ]]; then
                    chmod 664 "$file"
                elif [[ "$currentType" == "directory" ]]; then
                    chmod 755 "$file"
                fi
            fi

        elif [[ "$type" == "A" ]]; then
          if (( currentPerm & perm )); then
            if [[ "$currentType" == "regular file" ]]; then
                chmod 664 "$file"
            elif [[ "$currentType" == "directory" ]]; then
                chmod 755 "$file"
          fi
    fi
fi

       elif [[ "$type" == "T" ]]; then
    if (( (currentPerm & perm) == perm )); then
        if [[ "$currentType" == "regular file" ]]; then
            chmod 664 "$file"
        elif [[ "$currentType" == "directory" ]]; then
            chmod 755 "$file"
        fi
    fi
fi

    done < <(find "$path" -mindepth 1 \( -type f -o -type d \))

done < "$1"
