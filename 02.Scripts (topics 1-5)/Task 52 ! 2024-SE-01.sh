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
