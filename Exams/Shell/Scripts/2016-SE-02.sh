#Напишете shell скрипт, който приема един позиционен параметър - число. Ако
#скриптът се изпълнява като root, да извежда обобщена информация за общото количество активна
#памет (RSS - resident set size, non-swapped physical memory that a task has used) на процесите на
#всеки потребител. Ако за някой потребител обобщеното число надвишава подадения параметър, да
#изпраща подходящи сигнали за прекратяване на процеса с най-много активна памет на потребителя.
#Забележка: Приемаме, че изхода в колоната RSS е число в същата мерна единица, като числото,
#подадено като аргумент. Примерен формат:
#USER PID %CPU %MEM VSZ RSS TTY STAT START TIME COMMAND
#root 1 0.0 0.0 15816 1884 ? Ss May12 0:03 init [2]
#root 2 0.0 0.0 0 0 ? S May12 0:00 [kthreadd]
#root 3 0.0 0.0 0 0 ? S May12 0:02 [ksoftirqd/0]
#Забележка: Алтернативно може да ползвате изхода от ps -e -o uid,pid,rss

#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

limit=$1

if [[ "$(id -u) -ne 0]; then     #-u връща UID (user ID).За root винаги е 0.
  echo "Error: This script must be run as root."
  exit 2
fi

ps -e -o user=,pid=,rss= | awk -v LIMIT="$limit" '
{
  user=$1; pid=$2; rss=$3;
  total[user] +=rss;    #Натрупва колко общо памет (RSS) ползват всички процеси на даден user.
  if (rss > max_rss[user]) {   #Проверяваме дали текущият процес има по-голям RSS от предишния най-голям за този потребител.
    max_rss[user] = rss;
    max_pid[user] = pid;  #(PID-а на процеса с най-голямо RSS).
  }
}
END
{
  for (u in total) {     #обхождаме всички потребители.
    printf "user: %s, total rss : %d\n",u, total[u];
    if (total[u] > limit) {
     printf "  -> Over limit! Killing process %d (RSS=%d)\n", max_pid[u], max_rss[u];
            system("kill -9 " max_pid[u]);
           }
     }
 };
