#!/bin/bash

if [[ ${#} -ne 2 ]];
then
    echo "error"
    exit 1
fi

classes=$(mktemp)

while read -r header;
do
    decl=$(grep -E "^class[ ]+[a-zA-Z_][a-zA-Z0-9_]*([ ]*:[ ]*(private|protected|public)[ ]+[a-zA-Z_][a-zA-Z0-9_]*([ ]*,[ ]*(private|protected|public)[ ]+[a-zA-Z_][a-zA-Z0-9_]*)*)?$" "$header")

    if [[ -z "$decl" ]]; then
        continue
    fi

    className=$(echo "$decl" | cut -d ':' -f 1 | cut -d ' ' -f 2)
    echo "$className" >> "$classes"

    doesDerives=$(echo "$decl" | grep -o ":")

    if [[ "$doesDerives" == ":" ]];
    then
        descs=$(echo "$decl" | cut -d ':' -f 2 | tr ',' '\n')

        while read -r part;
        do
            parent=$(echo "$part" | sed -E 's/^[ ]*(private|protected|public)[ ]+//')

            if [[ -n "$parent" ]]; then
                echo "$parent" >> "$classes"
                echo "$parent -> $className" >> "$classes"
            fi
        done <<< "$descs"
    fi
done < <(find "$1" -type f -name "*.h")

sort -u "$classes" | dag-ger > "$2"

rm -f "$classes"
exit 0
