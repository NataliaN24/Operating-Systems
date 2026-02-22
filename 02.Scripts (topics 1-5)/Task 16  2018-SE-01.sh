#!/bin/bash

if [[ $# -ne 1 ]]; then
  echo "1 argument needed" >&2
  exit 1
fi

LOGDIR="${1}"

if [[ ! -d "$LOGDIR" ]]; then
  echo "Error: '$LOGDIR' is not a directory" >&2
  exit 2
fi

#find *.txt files
#print0  will print the path to this txt file

find "$LOGDIR" -type f -name "*.txt" -print0 | xargs -0 wc -l 
| awk '{ 
        lines = $1 
        n=split($2,parts,"/")   # n=size  parts is the array where we store the path
        friend=parts[n-1]      # friend's name is the last part of the array

        sum[friend] +=lines
        }
        END {
          for (f in sum)
            print sum[f],f
            }'
   | sort -nr | head -n 10 | awk '{print $2,$1}'
