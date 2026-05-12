#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <dir>" >&2
    exit 1
fi

dir="$1"

if [[ ! -d "$dir" ]]; then
    echo "Invalid directory" >&2
    exit 1
fi

files_count=$(find "$dir" -type f | wc -l)

if [[ "$files_count" -eq 0 ]]; then
    exit 0
fi

needed=$(( (files_count + 1) / 2 ))

tmp=$(mktemp)

trap 'rm -f "$tmp"' EXIT

find "$dir" -type f | while read -r file; do

    tr -c 'a-z' '\n' < "$file" |
    grep -v '^$' |
    sort |
    uniq -c |

    while read -r count word; do
        echo "$word $count $file"
    done

done > "$tmp"

cut -d ' ' -f1 "$tmp" | sort | uniq |

while read -r word; do

    total=0

    while read -r count; do
        total=$(( total + count ))
    done < <(
        grep "^$word " "$tmp" |
        cut -d ' ' -f2
    )

    good_files=0

    while read -r count; do

        if [[ "$count" -ge 3 ]]; then
            good_files=$(( good_files + 1 ))
        fi

    done < <(
        grep "^$word " "$tmp" |
        cut -d ' ' -f2
    )

    if [[ "$good_files" -ge "$needed" ]]; then
        echo "$total $word"
    fi

done |

sort -nr |
head -n 10 |
cut -d ' ' -f2
