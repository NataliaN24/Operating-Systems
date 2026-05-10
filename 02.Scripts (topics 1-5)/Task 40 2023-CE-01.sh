#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 <csv_file> <star_type>" >&2
    exit 1
fi

file="$1"
type="$2"

if [[ ! -f "$file" ]]; then
    echo "Error: file does not exist" >&2
    exit 1
fi

constellation=$(
    grep ",$type," "$file" |
    cut -d ',' -f4 |
    sort |
    uniq -c |
    sort -nr |
    head -n 1 |
    tr -s ' ' |
    cut -d ' ' -f3
)

if [[ -z "$constellation" ]]; then
    echo "Error: no stars of type $type" >&2
    exit 1
fi

grep ",$constellation," "$file" |
grep -v ',--$' |
sort -t ',' -k7,7n |
head -n 1 |
cut -d ',' -f1
