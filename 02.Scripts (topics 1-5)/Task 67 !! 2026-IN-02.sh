#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "error"
    exit 1
fi

input="$1"
k="$2"

if [[ ! -f "$input" ]]; then
    echo "error"
    exit 1
fi

if [[ ! "$k" =~ ^[0-9]+$ ]]; then
    echo "error"
    exit 2
fi

dict="/usr/share/dict/words"

if [[ ! -f "$dict" ]]; then
    echo "error"
    exit 1
fi

decoded=$(mktemp)
level=$(mktemp)
unique=$(mktemp)

alpha="A B C D E F G H I J K L M N O P Q R S T U V W X Y Z"

cnt=1

for letter in $alpha; do
    echo "$letter $cnt" >> "$level"
    cnt=$((cnt + 1))
done

while read -r line; do

    words=$(echo "$line" | grep -Eo '[A-Z]+')

    while read -r word; do

        len=$(echo -n "$word" | wc -c)

        i=1

        while [[ "$i" -le "$len" ]]; do

            letter=$(echo -n "$word" | cut -c "$i")

            currentPos=$(grep "^$letter " "$level" | cut -d ' ' -f2)

            newPos=$(( (currentPos - 1 + k) % 26 + 1 ))

            newLetter=$(grep " $newPos$" "$level" | cut -d ' ' -f1)

            echo -n "$newLetter" >> "$decoded"

            i=$((i + 1))
        done

        echo >> "$decoded"

    done <<< "$words"

done < "$input"

sort -u "$decoded" > "$unique"

howMany=0

while read -r word; do

    w=$(echo "$word" | tr 'A-Z' 'a-z')

    if grep -Fxi "$w" "$dict" > /dev/null; then
        howMany=$((howMany + 1))
    fi

done < "$unique"

echo "$howMany"

rm -f "$decoded" "$level" "$unique"
///////////////////////////////////
#!/bin/bash

if [[ $# -ne 2 ]]; then
    exit 2
fi

file="$1"
k="$2"

if [[ ! -f "$file" ]]; then
    exit 2
fi

if [[ ! "$k" =~ ^[0-9]+$ ]]; then
    exit 2
fi

echo -e "A 1\nB 2\nC 3\nD 4\nE 5\nF 6\nG 7\nH 8\nI 9\nJ 10\nK 11\nL 12\nM 13\nN 14\nO 15\nP 16\nQ 17\nR 18\nS 19\nT 20\nU 21\nV 22\nW 23\nX 24\nY 25\nZ 26" > alphabet

dict="/usr/share/dict/words"

words=$(tr ' ' '\n' < "$file" | tr -d '[:punct:]' | tr 'A-Z' 'a-z' | sort -u)

count=0

while read -r word; do
    decoded=""

    length=$(echo -n "$word" | wc -c)

    for ((i=0; i<length; i++)); do
        letter=$(echo -n "$word" | cut -c $((i+1)))

        letter=$(echo "$letter" | tr 'a-z' 'A-Z')

        position=$(grep "^$letter " alphabet | cut -d' ' -f2)

        new_position=$(( (position + k - 1) % 26 + 1 ))

        new_letter=$(grep " $new_position$" alphabet | cut -d' ' -f1)

        decoded="$decoded$new_letter"
    done

    decoded=$(echo "$decoded" | tr 'A-Z' 'a-z')

    if grep -qi "^$decoded$" "$dict"; then
        count=$((count + 1))
    fi

done <<< "$words"

echo "$count"

rm alphabet
//////////////////////////////////////////////////////////
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



