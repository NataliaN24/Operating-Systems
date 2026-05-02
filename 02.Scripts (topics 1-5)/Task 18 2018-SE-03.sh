#!/bin/bash

if [[ $# -ne 2 ]]; then
  exit 1
fi


if [[ ! -f "$1" || ! -f "$2"]];then
  exit 1;
fi 
in="$1"
out="$2"

while IFS= read -r line
do
  id=$(echo "$line" | cut -d ',' -f1)
  rest=$(echo "$line" | cut -d ',' -f2-)
  found=0

  while IFS= read -r existing; do
  
   old_id=$(echo "$existing" | cut -d ',' -f1)
   old_rest=$(echo "$existing" | cut -d ',' -f2-)
  
    if [[ "$rest" == "$old_rest" ]];then
    found=1
    if [[ "$id" -lt "$old_id" ]];then
      grep -v "^$old_id,$old_rest$" "$out" >tmp
      mv tmp "$out"

      echo "$line" >> "$out"
      fi
    fi
      
  done <"$out"
  if [[ $found -eq 0 ]]; then
      echo "$line" >> "$out"
  fi 

done < "$in"
