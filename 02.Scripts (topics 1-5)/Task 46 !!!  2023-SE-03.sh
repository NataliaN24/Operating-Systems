#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

if [[ ! -d "$1" ]]; then
    exit 2
fi

dir="$1"

tmp_words=$(mktemp)
tmp_file_words=$(mktemp)
tmp_total=$(mktemp)
tmp_file_counts=$(mktemp)

find "$dir" -type f | while read -r file; do
    grep -o '[a-z]\+' "$file" >> "$tmp_words"

    grep -o '[a-z]\+' "$file" | sort | uniq -c | while read -r cnt word; do
        if [[ "$cnt" -ge 3 ]]; then
            echo "$word" >> "$tmp_file_words"
        fi
    done
done

files_count=$(find "$dir" -type f | wc -l)

if [[ "$files_count" -eq 0 ]]; then
    rm "$tmp_words" "$tmp_file_words" "$tmp_total" "$tmp_file_counts"
    exit 0
fi

sort "$tmp_words" | uniq -c > "$tmp_total"
sort "$tmp_file_words" | uniq -c > "$tmp_file_counts"

while read -r file_cnt word; do
    if [[ $((file_cnt * 2)) -ge "$files_count" ]]; then
        total_cnt=$(grep " $word$" "$tmp_total" | tr -s ' ' | cut -d ' ' -f2)
        echo "$total_cnt $word"
    fi
done < "$tmp_file_counts" | sort -nr | head -n 10 | cut -d ' ' -f2

rm "$tmp_words" "$tmp_file_words" "$tmp_total" "$tmp_file_counts"
#########################################################################################


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
