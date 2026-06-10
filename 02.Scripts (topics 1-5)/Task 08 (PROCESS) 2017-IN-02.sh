#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

if [[ "$(whoami)" != "root" ]]; then
    exit 2
fi

userName="$1"

results=$(mktemp)
users=$(mktemp)
fooProcc=$(mktemp)

ps -eo user=,pid=,time= | tr -s ' ' | sed 's/^ //' > "$results"

cut -d ' ' -f1 "$results" | sort -u > "$users"

# a)
fooNumberProcc=$(grep "^$userName " "$results" | wc -l)

while read -r user; do
    numberOfProcc=$(grep "^$user " "$results" | wc -l)

    if [[ "$numberOfProcc" -gt "$fooNumberProcc" ]]; then
        echo "$user"
    fi
done < "$users"

# b)
numberOfProcesses=$(cat "$results" | wc -l)
sum=0

while read -r user pid time; do

    hours=$(echo "$time" | cut -d : -f1)
    minutes=$(echo "$time" | cut -d : -f2)
    seconds=$(echo "$time" | cut -d : -f3)

    totalSeconds=$((hours * 3600 + minutes * 60 + seconds))

    sum=$((sum + totalSeconds))

done < "$results"

avgTime=$((sum / numberOfProcesses))

echo "$avgTime"

# c)
grep "^$userName " "$results" > "$fooProcc"

double=$((avgTime * 2))

while read -r user pid time; do

    hours=$(echo "$time" | cut -d : -f1)
    minutes=$(echo "$time" | cut -d : -f2)
    seconds=$(echo "$time" | cut -d : -f3)

    totalSeconds=$((hours * 3600 + minutes * 60 + seconds))

    if [[ "$totalSeconds" -gt "$double" ]]; then
        kill -TERM "$pid"
    fi

done < "$fooProcc"

rm "$results"
rm "$users"
rm "$fooProcc"
###################################################################################

#!/bin/bash

if [[ $# -ne 1 ]];then
  exit 1
fi
foo="$1"

if [[ $(whoami) != "root" ]];then
  exit 2
fi

#да извежда имената на потребителите, които имат повече на брой процеси от FOO, ако има такива;
usersOnly=$(mktemp)
allProcesses=$(mktemp)
result=$(mktemp)

processesFoo=$(ps -eo user= | grep -c "^$foo$")
ps -eo user= | sort -u > "$usersOnly"
ps -eo user= | sort > "$allProcesses"

while read -r user;do
  if [[ "$user" == "$foo" ]];then
    continue;
  fi
   count=$(grep -c "^$user$" "$allProcesses")
   
  if [[ "$count" -gt "$processesFoo" ]];then
    echo "$user" >> "$result"
  fi
done < "$usersOnly"

cat "$result"
rm -f "$usersOnly" "$allProcesses" "$result"

#да извежда средното време (в секунди), за което са работили
#процесите на всички потребители
#на системата (TIME, във формат HH:MM:SS);   00:00:05

ps -eo time= | awk -F: '{ sum +=$1*3600 + $2*60 +$3; count++} END { if (count > 0) print sum/count}'

 ако съществуват процеси на FOO, които са работили над два пъти повече от
средното време,
скриптът да прекратява изпълнението им по подходящ начин.
avg=$(ps -eo time= | awk -F: '{ sum +=$1*3600 + $2*60 +$3; count++} END { if (count > 0) print sum/count}')
ps -u "$foo" -o pid=,time= | while read -r pid time;do
  seconds=(echo "$time" |awk -F: '{ print $1*3600 + $2*60 + $3 }')
  if [[ "$seconds" -gt $(( 2* avg)) ]];then
    kill "$pid"
  fi
done




done
