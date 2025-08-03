#Да се отпечатат PID на всички процеси, които имат повече деца от родителския си процес.

$ps -eo pid=,ppid= | awk '{childCount[$2]++; parent[$1]=$2; } END { for (pid in parent) {ppid = parent[pid]; if(childCount[pid] > childCount[ppid]) print pid; } }'

#or

ps -A -o pid,ppid | awk '{parent[$1]=$2;children[$2]++;} END{for (child in children) {if (children[child] > children[parent[child]]) print child}}'
