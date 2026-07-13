#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

records=$(mktemp)

# Събираме приятел и брой редове за всеки лог
while read -r file; do
    friend=$(basename "$(dirname "$file")")
    lines=$(wc -l < "$file")

    echo "$friend $lines" >> "$records"

done < <(find "$1" -type f | grep -E '[0-9]{4}-[0-9]{2}-[0-9]{2}-[0-9]{2}-[0-9]{4}\.txt$')


final=$(mktemp)

# Сумираме всички разговори с един и същ приятел
while read -r friend count; do

    current=$(grep "^$friend " "$final" | cut -d' ' -f2)

    if [[ -z "$current" ]]; then
        echo "$friend $count" >> "$final"
    else
        new=$((current + count))
        sed -i "s/^$friend .*/$friend $new/" "$final"
    fi

done < "$records"


sort -k2 -nr "$final" | head -n 10

#################################################3
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
