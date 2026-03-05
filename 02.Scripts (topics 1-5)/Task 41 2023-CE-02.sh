#!/bin/bash

if [[ $# -ne 3 ]];then
  exit 1;
fi

file1=${1}
file2=${2}
blackhole=${3}

if [[ ! -f "$file1" ]];then
  exit 1;
fi
if [[ ! -f "$file2" ]];then
  exit 1;
fi

found1=$(grep "^$blackhole:" "$file1"| head -n 1)
found2=$(grep "^$blackhole:" "$file2"| head -n 1)
if [[ -z "$found1" ]];then
  echo "$file2"
fi
if [[ -z "$found2" ]];then
  echo "$file1"
fi

if [[ -z "$found1" ]]&& [[ -z "$found2" ]];then
  exit 2
fi
dist1=$(cat "$file1" | cut -d ' ' -f2)
dist2=$(cat "$file2" | cut -d ' ' -f2)

if (( dist1 <= dist2 )); then
  echo "$file1"
else
  echo "$file2"
fi

