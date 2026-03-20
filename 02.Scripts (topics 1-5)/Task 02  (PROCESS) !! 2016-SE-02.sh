#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <number>" >&2
    exit 1
fi

limit="$1"

if [[ ! "$limit" =~ ^[0-9]+$ ]]; then
    echo "Argument must be a number" >&2
    exit 2
fi

if [[ $EUID -ne 0 ]]; then
    echo "Script must be run as root" >&2
    exit 3
fi

tmp=$(mktemp)
ps -eo user=,pid=,rss= | tr -s ' ' | sed 's/^ //' > "$tmp"

users=$(cut -d ' ' -f1 "$tmp" | sort | uniq)

for user in $users; do
    total=0
    maxrss=0
    maxpid=0

    while read -r curr_user pid rss; do
        if [[ "$curr_user" == "$user" ]]; then
            total=$(( total + rss ))

            if [[ "$rss" -gt "$maxrss" ]]; then
                maxrss="$rss"
                maxpid="$pid"
            fi
        fi
    done < "$tmp"

    echo "$user $total"

    if [[ "$total" -gt "$limit" ]]; then
        kill -TERM "$maxpid"
        sleep 1

        if kill -0 "$maxpid" 2>/dev/null; then
            kill -KILL "$maxpid"
        fi
    fi
done

rm -f "$tmp"
