#!/bin/bash
// A ->
maxVal=-1
allMaxValues=$(mktemp)

while read -r line; do
    num=$(echo "$line" | grep -E '^-?[0-9]+$')
    absolute=$(echo "$num" | tr -d '-')

    if [[ -z "$num" ]]; then
        continue
    fi

    if [[ "$absolute" -gt "$maxVal" ]]; then
        maxVal="$absolute"
        echo "$num" > "$allMaxValues"
    elif [[ "$absolute" -eq "$maxVal" ]]; then
        echo "$num" >> "$allMaxValues"
    fi
done

sort -n "$allMaxValues" | uniq
rm "$allMaxValues"
/////////////////////////////////////////////////////////////////////////////////////////////////////
#first script

tmp=$(mktemp)
grep -E '^-?[0-9]+$' > "$tmp"

maxAbs=0
while IFS= read -r n;do
  abs=$(echo "$n" | sed 's/^-//')   #remove the minus
  if [[ abs > maxAbs ]];then
    maxAbs="$abs"
  fi
  done < "$tmp"
  while IFS= read -r n;do
    abs=$(echo "$n" | sed 's/^-//')
     if (( abs == maxAbs )); then
    echo "$n"
  fi
done < "$tmp" | sort -n | uniq



#second script
#!/bin/bash

max_sum=-1
min_num=""

while IFS= read -r line; do

    # само цели числа
    if ! [[ "$line" =~ ^-?[0-9]+$ ]]; then
        continue
    fi

    num="$line"
    abs=$(echo "$line" | sed 's/-//')  # махаме минуса

    sum=0
    tmp="$abs"

    # взимаме цифра по цифра отляво
    while [[ -n "$tmp" ]]; do
        digit=$(echo "$tmp" | cut -c1)
        sum=$((sum + digit))
        tmp=$(echo "$tmp" | cut -c2-)
    done

    if [[ $sum -gt $max_sum ]]; then
        max_sum=$sum
        min_num="$num"
    elif [[ $sum -eq $max_sum && "$num" -lt "$min_num" ]]; then
        min_num="$num"
    fi

done

echo "$min_num"
