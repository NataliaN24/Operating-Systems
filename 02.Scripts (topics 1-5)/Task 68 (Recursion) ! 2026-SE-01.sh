#!/bin/bash

file="$1"

if [[ ! -f "$file" ]]; then
    exit 1
fi

include_func()
{
    local f="$1"
    local dir
    local line
    local newFile
    local path

    dir=$(dirname "$f")

    while read -r line; do
        if ! echo "$line" | grep -q "^!include:.+"; then
            echo "$line"
        else
            newFile=$(echo "$line" | cut -d ':' -f2-)

            path="$dir/$newFile"

            if [[ ! -f "$path" ]]; then
                exit 2
            fi

            include_func "$path"
        fi
    done < "$f"
}

include_func "$file"
/////////////////////////////
#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

file="$1"

if [[ ! -f "$file" || ! -r "$file" ]]; then
    exit 1
fi

tmp=$(mktemp)

processFile() {
    currentFile="$1"

    if [[ ! -f "$currentFile" || ! -r "$currentFile" ]]; then
        rm "$tmp"
        exit 1
    fi

    dir=$(dirname "$currentFile")

    while read -r line; do
        if echo "$line" | grep -q '^!include:'; then
            nextFile=$(echo "$line" | cut -d ':' -f2- | sed 's/^ *//; s/ *$//')
            processFile "$dir/$nextFile"
        else
            echo "$line" >> "$tmp"
        fi
    done < "$currentFile"
}

processFile "$file"

cat "$tmp"
rm "$tmp"
