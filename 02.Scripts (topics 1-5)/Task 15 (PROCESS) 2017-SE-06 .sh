#!/bin/bash

if [[ "$(id -u)" -ne 0 ]]; then
    exit 1
fi

users_file=$(mktemp)

while IFS=: read -r user pass uid gid gecos home shell; do
    [[ "$user" == "root" ]] && continue

    firstFlag=false
    secondFlag=false
    thirdFlag=false

    # 1) няма такава home директория
    if [[ ! -d "$home" ]]; then
        firstFlag=true
    else
        # 2) не е собственик на home директорията
        owner=$(stat -c '%U' "$home")
        if [[ "$owner" != "$user" ]]; then
            secondFlag=true
        fi

        # 3) собственикът няма право за писане
        perms=$(stat -c '%A' "$home")
        owner_write=${perms:2:1}
        if [[ "$owner_write" != "w" ]]; then
            thirdFlag=true
        fi
    fi

    if [[ "$firstFlag" == "true" || "$secondFlag" == "true" || "$thirdFlag" == "true" ]]; then
        echo "$user" >> "$users_file"
    fi
done < /etc/passwd

rootRSS=$(ps -u root -o rss= | awk '{sum += $1} END {print sum+0}')

while read -r user; do
    userRSS=$(ps -u "$user" -o rss= 2>/dev/null | awk '{sum += $1} END {print sum+0}')

    if [[ "$userRSS" -gt "$rootRSS" ]]; then
        ps -u "$user" -o pid= 2>/dev/null | while read -r pid; do
            kill -TERM "$pid"
        done
    fi
done < "$users_file"

rm -f "$users_file"
