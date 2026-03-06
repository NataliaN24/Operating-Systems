#!/bin/bash

if [[ $# -ne 2 ]]; then
    exit 1
fi

bad_words_file="$1"
dir="$2"

if [[ ! -f "$bad_words_file" ]]; then
    exit 2
fi

if [[ ! -d "$dir" ]]; then
    exit 3
fi


find "$dir" -type f -name '*.txt' | while read -r file;do
  while read -r word
  if [[ -z "$word" ]];then
    continue;
  fi
  censored=$(echo "$word" | sed 's/./*/g')
 if grep -qw "$word" "$file";then
   sed -i "s/\b$word\b/$censored/g' "$file"  #\b означава граница на дума (word boundary).
fi
  done< "$bad_words_file"
done
