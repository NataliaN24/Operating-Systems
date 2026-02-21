#!/bin/bash

if [[ $# -ne 3 ]];then
  echo "three args"
  exit 1;
fi
name=${1}
str1=${2}
str2=${3}

if [[ ! -f "$name" ]];then
  exit 2;
fi

line1=$(grep -F "^${str1}=" "$name"| cut -d '=' -f2 )
line2=$(grep -F "^${str2}=" "$name" | cut -d '=' -f2)

tmp1=$(mktemp)
tmp2=$(mktemp)

echo "$line1" | tr ' ' '\n' | sort > "$tmp1"
echo "$line2" | tr ' ' '\n' | sort > "$tmp2"
//c e f
//a b c d
newv=$(comm -23 "$tmp2" "$tmp1" | tr '\n' ' ')

sed -i "s/^${str2}=.*/${str2}=${newv}/" "$name"

rm "$tmp1" "$tmp2"
