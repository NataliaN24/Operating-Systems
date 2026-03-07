#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "error"
    exit 1
fi

if [[ ! -d "$1" ]]; then
    echo "error"
    exit 1
fi

words=$(mktemp)

allFiles=$(find "$1" -mindepth 1 -type f | wc -l)

if [[ "$allFiles" -eq 0 ]]; then
    rm -f "$words"
    exit 0
fi

while read -r file;do
      tr -cs  'a-z' '\n'  < "$file"  #всичко което НЕ е буква a-z → става нов ред
      | sed '/^$/d' |  sort | uniq -c     #махни празните редове
      | tr -s ' ' | sed 's/^//'  |grep -E '^[0-9]+ [a-z]+$' >>"$words"     #<spaces><count><space><word>
done< <(find "$1" -mindepth 1 -type f)

allFilesWords=$(mktemp)
awk '{ count[$2] += $1} END {for (word in count) print word, count[word] }' "$words" > "$allFilesWords"
uniqWords=$(mktemp)
cut -d '' -f2 "$words" | sort | uniw > "$uniqWords"
res=$(mktemp)

while read -r word;do  
  fileOccurs=$(awk -v w="$word" '$2 == w && $1 >=3 {c++} END {print c+0}' "$words")   #в колко файла тази дума се среща поне 3 пъти
  sum=$(awk -v w="$word" '$1 ==w {print $2}' "$allFilesWords")
  if [[ $((fileOccurs *2 )) -ge "$allFiles" ]];then
    echo "$word $sum" >> "$res"
    fi
done < "$uniqWords"

sort -k2,2 nr -k1,1 "$res |  head -n10 | cut -d ' ' -f1
rm -f "$words" "$allFilesWords" "$uniqWords" "$res"
