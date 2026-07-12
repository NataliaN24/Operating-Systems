#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

if [[ ! -f "$1" ]]; then
    exit 2
fi

tmp=$(mktemp)

number=1

while read -r line; do
    trimmedLine=$(echo "$line" | cut -d ' ' -f4-)
    echo "$number. $trimmedLine" >> "$tmp"
    number=$((number + 1))
done < "$1"

sort -k2 "$tmp"

rm "$tmp"
##############################################################


#!/bin/bash

if [[ $# -ne 1 ]]; then
  echo "one argument please"
  exit 1;
fi

if [[ ! -e $1 ]]; then
  echo "file doesn't exist"
  exit 2;
fi

tempFile=$(mktemp)

cut -d ' ' -f4- "${1}" | sort > tempFile    #from 4 th col till the end
i=0
while read line
do
  echo "$((++i)). "$line"
done < tempFile

rm tempFile
