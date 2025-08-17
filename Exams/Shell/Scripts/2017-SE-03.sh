#Напишете скрипт, който ако се изпълнява от root потребителя:
#а) извежда обобщена информация за броя и общото количество активна памет (RSS - resident set
#size, non-swaped physical memory that a task has used) на текущите процеси на всеки потребител;
#б) ако процесът с най-голяма активна памет на даден потребител използва два пъти повече памет
#от средното за потребителя, то скриптът да прекратява изпълнението му по подходящ начин.
#За справка:
#$ ps aux | head -5
#USER PID %CPU %MEM VSZ RSS TTY STAT START TIME COMMAND
#root 1 0.0 0.0 15820 1052 ? Ss Apr21 0:06 init [2]
#root 2 0.0 0.0 0 0 ? S Apr21 0:00 [kthreadd]
#root 3 0.0 0.0 0 0 ? S Apr21 0:02 [ksoftirqd/0]
#root 5 0.0 0.0 0 0 ? S< Apr21 0:00 [kworker/0:0H]
#Алтернативно, може да ползвате изхода от ps -e -o uid,pid,rss

#!/bin/bash

if [[ $EUID -ne 0 ]]; then
    echo "This script must be run as root."
    exit 1
fi

ps -e -o user=,pid=,rss= | awk '
{
    user=$1
    pid=$2
    rss=$3

    count[user]++
    total_rss[user]+=$3

    if (rss > max_rss[user]) {
        max_rss[user]=rss
        max_pid[user]=pid
    }
    users[user]=1
}
END {
    for (u in users) {
        avg=total_rss[u]/count[u]
        printf "User: %s | Processes: %d | Total RSS: %d KB | Average RSS: %.2f KB\n", u, count[u], total_rss[u], avg
      
        if (max_rss[u] >= 2*avg) {
            printf "  Killing process PID %d (RSS %d KB) for user %s\n", max_pid[u], max_rss[u], u
            system("kill -15 " max_pid[u])
        }
    }
}'
