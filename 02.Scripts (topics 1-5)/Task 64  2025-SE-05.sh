#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

dir="$1"

if [[ ! -d "$dir" ]]; then
    exit 2
fi

user=$(whoami)
dirGroup=$(stat -c "%G" "$dir")

if [[ "$user" == "root" ]]; then
    while read -r file; do
     chown :"$dirGroup" "$file"
        if [[ -d "$file" ]]; then
            chmod 770 "$file"
        elif [[ -f "$file" ]]; then
            chmod u+rw,g+rw,o-rwx "$file"
        fi
    done < <(find "$dir")

    exit 0
fi

userGroups=$(id -nG "$user")

found=$(echo "$userGroups" | tr ' ' '\n' | grep -x "$dirGroup")

if [[ -z "$found" ]]; then
    exit 3
fi

umask 007
