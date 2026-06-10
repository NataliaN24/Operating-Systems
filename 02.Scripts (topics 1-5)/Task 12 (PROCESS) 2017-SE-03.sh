#!/bin/bash

if [[ "$(whoami)" != "root" ]]; then
    exit 1
fi

results=$(mktemp)
users=$(mktemp)

ps -eo uid=,pid=,rss= | tr -s ' ' | sed 's/^ //' > "$results"
ps -eo uid= | tr -s ' ' | sed 's/^ //' | sort -u > "$users"

while read -r user; do

    sum=0
    count=0
    maxRss=0
    maxPID=""

    while read -r uid pid rss; do
        if [[ "$uid" == "$user" ]]; then
            count=$((count + 1))
            sum=$((sum + rss))

            if [[ "$rss" -gt "$maxRss" ]]; then
                maxRss="$rss"
                maxPID="$pid"
            fi
        fi
    done < "$results"

    echo "user: $user process count: $count total RSS: $sum"

    avg=$((sum / count))
    doubleAVG=$((avg * 2))

    if [[ "$maxRss" -gt "$doubleAVG" ]]; then
        kill -TERM "$maxPID"
    fi

done < "$users"

rm "$results" "$users"
############################################################################################################################################################





#!/bin/bash

if [[ "$(id -u)" -ne 0 ]]; then
    exit 1
fi

ps -eo user=,pid=,rss= | awk '
{
    user = $1
    pid  = $2
    rss  = $3

    count[user]++
    sum[user] += rss

    if (!(user in maxrss) || rss > maxrss[user]) {
        maxrss[user] = rss
        maxpid[user] = pid
    }
}
END {
    for (u in count) {
        print u, count[u], sum[u], maxrss[u], maxpid[u]
    }
}' | while read -r user count sum maxrss maxpid; do
    echo "$user $count $sum"

    if (( maxrss * count > 2 * sum )); then
        kill -TERM "$maxpid"
    fi
done
