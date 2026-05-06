#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 logfile"
    exit 1
fi

file="$1"

if [[ ! -f "$file" ]]; then
    echo "File does not exist"
    exit 1
fi

tmp=$(mktemp)

# top 3 сайта с най-много заявки
cut -d ' ' -f2 "$file" |
sort |
uniq -c |
sort -nr |
head -n 3 |
awk '{print $2}' > "$tmp"

while read -r site; do

    http2=$(grep " $site " "$file" | grep "HTTP/2.0" | wc -l)

    non_http2=$(grep " $site " "$file" | grep -v "HTTP/2.0" | wc -l)

    echo "$site HTTP/2.0: $http2 non-HTTP/2.0: $non_http2"

    grep " $site " "$file" |
    awk '$9 > 302 { print $1 }' |
    sort |
    uniq -c |
    sort -nr |
    head -n 5

done < "$tmp"

rm -f "$tmp"
