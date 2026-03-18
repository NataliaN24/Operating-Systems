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



