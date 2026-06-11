#!/bin/bash

if [[ $# -ne 2 ]]; then
    exit 1
fi

file="$1"
num="$2"

if [[ ! -f "$file" ]]; then
    exit 2
fi

if [[ ! "$num" =~ ^[0-9]+$ ]]; then
    exit 2
fi

dict="/usr/share/dict/words"

if [[ ! -f "$dict" ]]; then
    exit 3
fi

validWords=$(mktemp)
capitalWords=$(mktemp)
decoded=$(mktemp)

while read -r line; do
    echo "$line" | grep -Eo '[A-Z]+' >> "$capitalWords"
done < "$file"

while read -r word; do
    echo "$word" | tr 'A-Z' 'a-z' >> "$validWords"
done < "$capitalWords"

abc="abcdefghijklmnopqrstuvwxyz"
num=$((num % 26))

while read -r word; do
    newWord=""
    length=$(echo -n "$word" | wc -c)

    for (( i=0; i<length; i++ )); do
        letter=$(echo -n "$word" | cut -c $((i + 1)))

        currPos=0

        for (( j=0; j<26; j++ )); do
            currLetter=$(echo -n "$abc" | cut -c $((j + 1)))

            if [[ "$currLetter" == "$letter" ]]; then
                currPos="$j"
            fi
        done

        newPos=$(( (currPos + num) % 26 ))
        newLetter=$(echo -n "$abc" | cut -c $((newPos + 1)))
        newWord="$newWord$newLetter"
    done

    echo "$newWord" >> "$decoded"
done < "$validWords"

count=0

while read -r word; do
    if grep -qix "$word" "$dict"; then
        count=$((count + 1))
    fi
done < <(sort -u "$decoded")

echo "$count"

rm "$validWords" "$capitalWords" "$decoded"
#######################################################################################################





#!/bin/bash

if [[ $# -ne 2 ]];then
  exit 1
fi

path="$1"
num="$2"

# /usr/share/dict/words

if [[ ! -f "$path" ]];then
  exit 2
fi

if [[ ! "$num"  =~ ^[0-9]+$ ]];then
  exit 3
fi

wordsFile=$(mktemp)
egrep -o '[A-Z]+' "$path" | sort -u > "$wordsFile"

library="/usr/share/dict/words"

#${abc:$k}  започни от позиция 3 (индексирането започва от 0!)
#${variable:начало:дължина}
#DEFGHIJKLMNOPQRSTUVWXYZABC

abc="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
num=$(( num %26 ))
shifted="${abc:$num}${abc:0:$num}"

 count=0;
 
while read -r word;do
  newWord=$(echo "$word" | tr "$abc" "$shifted") 
  
  if grep -i -x -q "$newWord" "$library"; then
    count=$((count + 1))
  fi
  
done < "$wordsFile"

 rm -f "$wordsFile" 
 echo "$count"



