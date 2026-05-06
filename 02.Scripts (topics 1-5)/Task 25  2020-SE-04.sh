#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 SRC DST"
    exit 1
fi

src="$1"
dst="$2"

if [[ ! -d "$src" ]]; then
    echo "SRC is not directory"
    exit 1
fi

if [[ -e "$dst" ]]; then
    echo "DST already exists"
    exit 1
fi

mkdir -p "$dst/images"

find "$src" -type f -name "*.jpg" | while read -r file; do

    filename=$(basename "$file")

    # махаме .jpg
    name="${filename%.jpg}"

    # title
    title=$(echo "$name" |
        sed -E 's/\([^)]*\)//g' |
        tr -s ' ' |
        sed -E 's/^ //; s/ $//')

    # album
    album=$(echo "$name" |
        grep -o '([^)]*)' |
        tail -n 1 |
        tr -d '()' |
        tr -s ' ' |
        sed -E 's/^ //; s/ $//')

    if [[ -z "$album" ]]; then
        album="misc"
    fi

    # date
    date=$(stat -c '%y' "$file" | cut -d ' ' -f1)

    # hash
    hash=$(sha256sum "$file" | cut -c1-16)

    # копие на файла
    cp "$file" "$dst/images/${hash}.jpg"

    # директории
    p1="$dst/by-date/$date/by-album/$album/by-title"
    p2="$dst/by-date/$date/by-title"
    p3="$dst/by-album/$album/by-date/$date/by-title"
    p4="$dst/by-album/$album/by-title"
    p5="$dst/by-title"

    mkdir -p "$p1" "$p2" "$p3" "$p4" "$p5"

    # symlink-ове
    ln -s "../../../../../images/${hash}.jpg" \
        "$p1/${title}.jpg"

    ln -s "../../../images/${hash}.jpg" \
        "$p2/${title}.jpg"

    ln -s "../../../../../images/${hash}.jpg" \
        "$p3/${title}.jpg"

    ln -s "../../../images/${hash}.jpg" \
        "$p4/${title}.jpg"

    ln -s "../images/${hash}.jpg" \
        "$p5/${title}.jpg"

done
