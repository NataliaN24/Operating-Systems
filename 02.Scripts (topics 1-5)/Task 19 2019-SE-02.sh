#!/bin/bash

if [[ $# -eq 0 ]]; then
  echo "Script needs at least 1 argument..." >&2
  exit 2
fi

N=10
if [[ "${1}" -eq "-n" ]]; then
  if [[ $# -lt 3]]; then
   echo "Usage: $0 [-n N] FILE1.log [FILE2.log ...]" >&2
    exit 2
  fi
  if  ! [[  "${2}" =~ ^[0-9]+$ ]] || [[ "${2}" -le 0]];then
   echo "Error: N must be a positive integer." >&2
    exit 2
  fi
  N="${2}"
  shift 2        #this will make $1=file1 $2=file2 .....
fi
tempFile=$(mktemp);

for file in "$@" ;do
  if [[ ! -f "$file" ]];then
  echo "Error: '$file' is not a file." >&2
    rm -f "$tempFile"
    exit 2
  fi
   if [[ "$file" != *.log ]]; then
    echo "Error: '$file' must end with .log" >&2
    rm -f "$tempFile"
    exit 2
  fi

    idf="$(basename "$file".log)"
    while IFS= read -r line
    do
    timestamp=$(echo "$line" | cut -d ' ' -f1,2)
    data=$(echo "$line" | cut -d ' ' -f3-)
    echo "${timestamp}" ${idf} ${data}" >> tempFile
  
  done < <(tail -n "$N" "$file")

  sort "$tempFile"
  rm -f "$tempFile"
  
