#Напишете скрипт, който приема задължителен позиционен аргумент -
име на потребител FOO. Ако скриптът се изпълнява от root:
#а) да извежда имената на потребителите, които имат повече на брой процеси от FOO,
ако има
#такива;
#б) да извежда средното време (в секунди), за което са работили процесите
на всички потребители
#на системата (TIME, във формат HH:MM:SS);
#в) ако съществуват процеси на FOO, които са работили над два
пъти повече от средното време,
#скриптът да прекратява изпълнението им по подходящ начин.
#За справка:
#$ ps -e -o user,pid,%cpu,%mem,vsz,rss,tty,stat,time,command | head -5
#USER PID %CPU %MEM VSZ RSS TT STAT TIME COMMAND
#root 1 0.0 0.0 15820 1920 ? Ss 00:00:05 init [2]
#root 2 0.0 0.0 0 0 ? S 00:00:00 [kthreadd]
#root 3 0.0 0.0 0 0 ? S 00:00:01 [ksoftirqd/0]
#root 5 0.0 0.0 0 0 ? S< 00:00:00 [kworker/0:0H]


#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <username>"
    exit 1
fi

FOO="$1"

if [[ $EUID -ne 0 ]]; then
    echo "This script must be run as root."
    exit 2
fi

# Броим процесите на всеки потребител
ps -e -o user= | sort | uniq -c | while read count user; do
    if [[ "$user" == "$FOO" ]]; then
        foo_count=$count
    fi
done


echo "Users with more processes than $FOO:"
ps -e -o user= | sort | uniq -c | while read count user; do
    if [[ "$user" != "$FOO" && "$count" -gt "$foo_count" ]]; then
        echo "$user"
    fi
done

# б) Средно време на процесите на всички потребители

total_sec=0
proc_count=0

ps -e -o time= | while read t; do
    # TIME във формат HH:MM:SS или MM:SS
    IFS=: read -r h m s <<< $(echo "$t" | awk -F: '{if(NF==3) print $1,$2,$3; else print 0,$1,$2}')
    sec=$((10#$h*3600 + 10#$m*60 + 10#$s))
    total_sec=$((total_sec + sec))
    proc_count=$((proc_count + 1))
done

# Средно време
avg_sec=$((total_sec / proc_count))
echo "Average process time: $avg_sec seconds"

# в) Прекратяване на процесите на FOO, които са работили > 2*средното
ps -u "$FOO" -o pid=,time= | while read pid time; do
    IFS=: read -r h m s <<< $(echo "$time" | awk -F: '{if(NF==3) print $1,$2,$3; else print 0,$1,$2}')
    sec=$((10#$h*3600 + 10#$m*60 + 10#$s))
    if (( sec > 2*avg_sec )); then
        echo "Killing PID $pid of $FOO (time: $sec sec > 2*avg=$avg_sec)"
        kill -TERM "$pid"
    fi
done

#or
#!/bin/bash

if [[ $# -ne 1 ]] ; then
    echo "Expected 1 argument - username"
    exit 1
fi

if [[ "$EUID" -eq 0 ]] ; then
    echo "Script must be executed as root"
    exit 1
fi

prCount=$(ps -e -o user,pid | grep "${1}" | wc -l)
ps -e -o user,pid | awk '{print $1}' | sort | uniq -c | awk -v var=${prCount} '{if ($(NF-1) > var) print $NF}'
counter=$(ps -e -o pid | wc -l)

for time in $(ps -e -o pid,time | awk '{print $2}' | tail -n +2) ; do
    timestamp=$(date -u -d "1970-01-01 ${time}" +%s)
    seconds=$(( timestamp % 86400 ))
    totalSeconds=$(( totalSeconds + seconds ))
done
totalSeconds=$(( totalSeconds / counter ))

time_value=$(date -u -d "@${totalSeconds}" +'%H:%M:%S')

totalSeconds=$(( totalSeconds * 2 ))
for process in $(ps -e -o user,pid,time | grep "${1}" | tail -n +2) ; do
    pr=$(echo "${process}" | awk '{print $2}')
    time=$(echo "${process}" | awk '{print $3}')
    timestamp=$(date -d "1970-01-01 ${time}" +%s)
    seconds=$(( timestamp % 86400 ))
    totalSeconds1=$(( totalSeconds + seconds ))
    if [[ ${totalSeconds1} -gt ${totalSeconds} ]] ; then
                echo "${pr}"
    fi
done
