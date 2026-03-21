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
