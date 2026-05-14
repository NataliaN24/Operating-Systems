#!/bin/bash

if [[ $# -eq 0 ]]; then
    echo "No arguments"
    exit 1
fi

files=$(mktemp)
rules=$(mktemp)
markers=$(mktemp)

for arg in "$@"; do

    if [[ "$arg" == -R*=* ]]; then
        echo "$arg" >> "$rules"

    elif [[ "$arg" != -* ]]; then
        if [[ ! -f "$arg" ]]; then
            echo "Invalid file: $arg"
            exit 1
        fi

        echo "$arg" >> "$files"

    else
        echo "Invalid argument: $arg"
        exit 1
    fi

done

if [[ ! -s "$files" || ! -s "$rules" ]]; then
    echo "Need at least one file and one replacement"
    exit 1
fi

while read -r rule; do
    left=$(echo "$rule" | sed 's/^-R//' | cut -d '=' -f1)
    right=$(echo "$rule" | cut -d '=' -f2)

    marker=$(pwgen 32 1)

    echo "$left=$right=$marker" >> "$markers"

done < "$rules"

while read -r file; do

    while read -r line; do
        old=$(echo "$line" | cut -d '=' -f1)
        marker=$(echo "$line" | cut -d '=' -f3)

        sed -i -E "s/\b$old\b/$marker/g" "$file"

    done < "$markers"

    while read -r line; do
        new=$(echo "$line" | cut -d '=' -f2)
        marker=$(echo "$line" | cut -d '=' -f3)

        sed -i -E "s/\b$marker\b/$new/g" "$file"

    done < "$markers"

done < "$files"

rm "$files" "$rules" "$markers"
###########################################################################################
#!/bin/bash

rules=$(mktemp)
files=$(mktemp)

for arg in "$@"; do
    if echo "$arg" | grep -Eq '^-R[A-Za-z0-9]+=[A-Za-z0-9]+$'; then
        left=$(echo "$arg" | cut -c 3- | cut -d '=' -f 1)
        right=$(echo "$arg" | cut -c 3- | cut -d '=' -f 2)
        temp=$(pwgen 10 1)
        echo "$left $temp $right" >> "$rules"
    else
        echo "$arg" >> "$files"
    fi
done

while read -r file; do
    cp "$file" "${file}.tmp"

    while read -r left temp right; do
        sed -E "/^#/! s/\<${left}\>/${temp}/g" "${file}.tmp" > "${file}.new"
        mv "${file}.new" "${file}.tmp"
    done < "$rules"

    while read -r left temp right; do
        sed -E "/^#/! s/\<${temp}\>/${right}/g" "${file}.tmp" > "${file}.new"
        mv "${file}.new" "${file}.tmp"
    done < "$rules"

    mv "${file}.tmp" "$file"
done < "$files"

rm -f "$rules" "$files"
