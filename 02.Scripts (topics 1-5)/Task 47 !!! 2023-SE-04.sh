#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <dir>"
    exit 1
fi

if [[ ! -d "$1" ]]; then
    echo "The argument must be a directory"
    exit 1
fi

dir="$1"

tmp=$(mktemp)

while IFS= read -r file;do
    hash=$(sha256sum "$file" | cut -d ' ' -f1)
    size=$(stat -c '%s' "$file")
    echo "$hash|$size|$file" >> "$tmp"
done< < (find "$1" -mindepth 1 -type f)

sorted=$(mktemp)
sort "$tmp" > "$sorted"
currentHash="" 
currentSize=0
count=0
original=""
duplicates=()  #array
groups=0;
saved=0;

while IFS='|' read -r hash size file;do
  if [[ "$hash" =="currentHash"]];then
    count=$((count+1))
    duplicates+=("$file")
  else
    if [[ $count -ge 2]];then
      groups=$((groups+1))
      saved=$((saved +(count-1 ) *currentSize))

      for duplicates in "${duplicates[@]}";do
      rm -f "$duplicates"
      ln "$original" "$duplicate"
      done
    fi
  currentHash="$hash"
  currentSize="$size"
  count=1;
  original="$file"
  duplicates=()
  
done< "$sorted"

if [[ $count -ge 2 ]]; then
    groups=$((groups + 1))
    saved=$((saved + (count - 1) * current_size))

    for duplicate in "${duplicates[@]}"; do
        rm -f "$duplicate"
        ln "$original" "$duplicate"
    done
fi

echo "Deduplicated groups: $groups"
echo "Saved bytes: $saved"

rm -f "$tmp" "$sorted"
