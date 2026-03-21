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
