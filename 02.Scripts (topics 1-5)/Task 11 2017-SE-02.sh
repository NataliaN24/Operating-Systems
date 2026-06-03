#!/bin/bash

if [[ $# -ne 3 ]]; then
    exit 1
fi

src="$1"
dst="$2"
str="$3"

if [[ "$(whoami)" != "root" ]]; then
    exit 1
fi

if [[ ! -d "$src" || ! -d "$dst" ]]; then
    exit 1
fi

find "$src" -type f | while read -r file; do

    if echo "$file" | grep -q "$str"; then

        relpath=$(echo "$file" | sed "s#^$src/##")

        dir=$(dirname "$relpath")

        mkdir -p "$dst/$dir"

        mv "$file" "$dst/$relpath"

    fi

done
