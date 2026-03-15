#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

dir="$1"

if [[ ! -d "$dir" ]]; then
    exit 2
fi

dataDir="$dir/.data"

if [[ ! -d "$dataDir" ]]; then
    mkdir "$dataDir"
fi

while read -r file; do

    # пропускаме файловете в .data
    if [[ "$file" == "$dataDir/"* ]]; then
        continue
    fi

    shaSum=$(sha256sum "$file" | cut -d ' ' -f 1)
    target="$dataDir/$shaSum"

    # ако няма такъв файл в .data, местим го там
    if [[ ! -f "$target" ]]; then
        mv "$file" "$target"
    else
        rm "$file"
    fi

    # правим относителен път до .data
    parent=$(dirname "$file")
    relPath=""

    while [[ "$parent" != "$dir" ]]; do
        relPath="../$relPath"
        parent=$(dirname "$parent")
    done

    ln -s "${relPath}.data/$shaSum" "$file"

done < <(find "$dir" -type f)
