#!/bin/bash

number=0
file="$1/dict.txt"

touch "$file"

while read -r line; do

    name=$(echo "$line" | cut -d ' ' -f1)
    last=$(echo "$line" | cut -d ' ' -f2)

    reply=$(echo "$line" | sed 's/([^)]*)//' | tr -s ' ' | cut -d ':' -f2-)


    if grep -Eq "^$name $last;" "$file"; then

        num=$(grep -E "^$name $last;" "$file" | cut -d ';' -f2)

        echo "$reply" >> "$1/$num.txt"

    else

        number=$((number+1))

        echo "$name $last;$number" >> "$file"

        touch "$1/$number.txt"

        echo "$reply" >> "$1/$number.txt"

    fi

done < "$2"
###########################################################

#!/bin/bash

if [[ $# -ne 2 ]]; then
  echo "Usage: $0 <file> <directory> " >&2
  exit 1
fi

file="${1}"
dir="${2}"

if [[ ! -f "$file" || ! -d "$dir" ]];then
  exit 1;
fi 

if [[ -n "$(find "$dir" -mindepth 1 -maxdepth 1)" ]];then
  echo "must be empty"
  exit 2
fi

touch "${dir}/dict.txt"
num=1

while IFS= read -r line;
do
  name=$(echo "${line}" | cut -d : -f1| tr -s ' ' | cut -d ' ' -f1,2)

  # ако го няма в dict.txt → добавяме
  if ! grep -E -q "^${name} "${dir}/dict.txt"
    then
      echo "${name};${num}" >> "${dir}/dict.txt"
      touch "${dir}/${num}.txt"
      num=$(( num+1 ))
    fi
    currnum=$(grep "^${name};" "${dir}/dict.txt" | cut -d ':' -f2)
    echo "$line" >> "${dir}/${currnum}.txt"
done < "${file}"




  

