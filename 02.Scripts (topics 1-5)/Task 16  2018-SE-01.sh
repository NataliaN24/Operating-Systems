#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 LOGDIR"
    exit 1
fi

LOGDIR="$1"
TMP="temp.txt"

# събираме всички friend + lines
find "$LOGDIR" -type f | while read file; do
    friend=$(basename "$(dirname "$file"))
    lines=$(cat "$file" | wc -l)

    echo "$friend $lines" >> "$TMP"
done

# работим върху temp файла
awk '{ sum[$1] += $2 } END { for (f in sum) print sum[f], f }' "$TMP" |
sort -rn |
head -n 10

# трием temp файла
rm "$TMP"
