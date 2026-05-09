
#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 dir1 dir2" >&2
    exit 1
fi

src="$1"
dst="$2"

if [[ ! -d "$src" ]]; then
    echo "Error: $src is not a directory" >&2
    exit 1
fi

if [[ ! -d "$dst" ]]; then
    echo "Error: $dst is not a directory" >&2
    exit 1
fi

if [[ -n "$(ls -A "$dst")" ]]; then
    echo "Error: $dst is not empty" >&2
    exit 1
fi

find "$src" -type f | while read file; do
    dir=$(dirname "$file")
    name=$(basename "$file")

    copy=1

    if [[ "$name" == .*".swp" ]]; then
        original=$(echo "$name" | sed 's/^\.//' | sed 's/\.swp$//')

        if [[ -f "$dir/$original" ]]; then
            copy=0
        fi
    fi

    if [[ "$copy" -eq 1 ]]; then
        relative=$(echo "$file" | sed "s|^$src/||")

        mkdir -p "$dst/$(dirname "$relative")"
        cp "$file" "$dst/$relative"
    fi
done
