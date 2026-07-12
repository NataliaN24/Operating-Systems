#!/bin/bash

if [[ $# -ne 2 ]]; then
    exit 1
fi

file1="$1"
file2="$2"

count1=$(grep -c "$file1" "$file1")
count2=$(grep -c "$file2" "$file2")

if [[ "$count1" -gt "$count2" ]]; then
    winner="$file1"
else
    winner="$file2"
fi

output="$winner.songs"

cut -d' ' -f4- "$winner" | sort > "$output"
###############################################################################


#!/bin/bash

if [[ $# -ne 2 ]]; then
  echo "it must contain 2 arguments"
  exit 1;
fi
if [[ ! -e ${1} || ! -e ${2} ]];then
  echo "one of the files doesnt exist"
  exit 2
fi

linesForFile1=$(cat ${1} | grep -E "${1}" | wc -l)
linesForFile2=$(cat ${2} | grep -E "${2}" | wc -l)

if [[ "$linesForFile1" -gt "$linesForFile2" ]]; then
  touch "${1}.songs"
  cut -d ' ' -f 4 -"${1}" | sort > "${1}.songs"
else
    touch "${2}.songs"
  cut -d ' ' -f 4 -"${2}" | sort > "${2}.songs"
fi
exit 0
