#!/bin/bash

# check for args
if [[ $# -ne 2 ]]; then
    exit 1
fi

cmd="$1"
file="$2"

if [[ ! -f "$file" ]]; then
    touch "$file"
fi

currentTime=$(date '+%Y-%m-%d %H:%M %A')
# 2026-05-16 12:05 Saturday

result=$($cmd)
status=$?

if [[ "$status" -ne 0 ]]; then
    exit 3
fi

date=$(echo "$currentTime" | cut -d ' ' -f1)
time=$(echo "$currentTime" | cut -d ' ' -f2)
day=$(echo "$currentTime" | cut -d ' ' -f3)
hour=$(echo "$time" | cut -d ':' -f1)

tmp=$(mktemp)

grep "$day" "$file" | grep " $hour:" > "$tmp"

sum=0
count=0

while read -r line; do
    value=$(echo "$line" | cut -d ' ' -f4)
    sum=$(echo "$sum + $value" | bc)
    count=$((count + 1))
done < "$tmp"

if [[ "$count" -gt 0 ]]; then
    avg=$(echo "scale=10; $sum / $count" | bc)
    twiceAvg=$(echo "scale=10; $avg * 2" | bc)
    halfAvg=$(echo "scale=10; $avg / 2" | bc)

    abnormal=$(echo "$result < $halfAvg || $result > $twiceAvg" | bc)

    if [[ "$abnormal" -eq 1 ]]; then
        echo "$date $hour: $result abnormal"
        echo "$currentTime $result" >> "$file"
        rm "$tmp"
        exit 2
    fi
fi

echo "$currentTime $result" >> "$file"
rm "$tmp"
###########################################################################################################################
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
