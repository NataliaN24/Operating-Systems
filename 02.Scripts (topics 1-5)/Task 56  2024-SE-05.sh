#!/bin/bash
if [[ $# -ne 2 ]];then
  exit 1 
fi

command=${1}
histFile=${2}

value=$($command)
status=$?

if [[ $status -ne 0 ]]; then
  exit 3
fi

date=$(date +'%Y-%m-%d')
hour=$(date +'%H')
weekday=$(date +'%u')

#echo "$weekday|$hour|$date|$value" >> "$histFile"

# намираме старите записи
history=$(grep "^$weekday|$hour|" "$histFile" 2>/dev/null)

count=0
howMany=0

for line in $history; do
    val=$(echo "$line" | cut -d '|' -f4)
    count=$(echo "$count + $val" | bc)
    howMany=$((howMany + 1))   
done

if [[ $howMany -gt 0 ]]; then
    avg=$(echo "$count / $howMany" | bc -l)

    cond=$(echo "$value > 2*$avg" | bc)

    if [[ $cond -eq 1 ]]; then
        echo "$date $hour: $value abnormal"
        echo "$weekday|$hour|$date|$value" >> "$histFile"
        exit 2
    fi
fi

echo "$weekday|$hour|$date|$value" >> "$histFile"
