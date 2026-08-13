#!/bin/bash

if [[ $# -lt 1 ]];then
  exit 1
fi
limit=${1}
shift

start=$(date +%s.%N)

while true;do
  "$@"  #изпълни всички аргументи като команда
  count=$((count+1))
  now=$(date +%s.%N)
  elapsed=$(echo "now" - "start" | bc)
  if (( $(echo "$elapsed >= $limit" | bc -l) ));then
    break
  fi
done

total=$(echo "$now -$start" | bc)
avg=$(echo "$total /$count" |bc -l)
echo "Ran the command '$*' $count times for $total seconds."
printf "Average runtime: %.2f seconds.\n" "$avg"
