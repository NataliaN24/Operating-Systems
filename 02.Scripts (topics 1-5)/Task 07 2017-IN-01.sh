#!/bin/bash

if [[ $# -ne 3 ]]; then
    exit 1
fi

file="$1"
str1="$2"
str2="$3"

if [[ ! -f "$file" ]]; then
    exit 2
fi

tmp=$(mktemp)

# Намираме стойността на str1
terms1=""

while read -r line; do
    if echo "$line" | grep -q "^$str1="; then
        terms1=$(echo "$line" | cut -d '=' -f2-)
        break
    fi
done < "$file"

# Обработваме файла
while read -r line; do

    if ! echo "$line" | grep -q "^$str2="; then
        echo "$line" >> "$tmp"
        continue
    fi

    terms2=$(echo "$line" | cut -d '=' -f2-)

    finalTerms=""

    for opt in $terms2; do
        found=0

        for opt2 in $terms1; do
            if [[ "$opt" == "$opt2" ]]; then
                found=1
                break
            fi
        done

        if [[ "$found" -eq 0 ]]; then
            finalTerms="$finalTerms $opt"
        fi
    done

    finalTerms=$(echo "$finalTerms" | xargs)

    echo "$str2=$finalTerms" >> "$tmp"

done < "$file"

mv "$tmp" "$file"
########################################################

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
