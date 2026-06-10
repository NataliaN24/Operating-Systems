#!/bin/bash

if [[ "$(whoami)" != "root" ]]; then
    exit 1
fi

all=$(mktemp)
rootOnly=$(mktemp)
notRootOnly=$(mktemp)
notRootUsers=$(mktemp)

ps -eo user=,uid=,pid=,rss= | tr -s ' ' | sed 's/^ //' > "$all"

grep "^root " "$all" > "$rootOnly"
grep -v "^root " "$all" > "$notRootOnly"
cut -d ' ' -f2 "$notRootOnly" | sort -u > "$notRootUsers"

totalRSSroot=0

while read -r user uid pid rss; do
    totalRSSroot=$((totalRSSroot + rss))
done < "$rootOnly"

while read -r userID; do
    requirementFulfilled=0

    passwdLine=$(grep "^[^:]*:[^:]*:$userID:" /etc/passwd)

    if [[ -z "$passwdLine" ]]; then
        continue
    fi

    userName=$(echo "$passwdLine" | cut -d ':' -f1)
    homeDir=$(echo "$passwdLine" | cut -d ':' -f6)

    if [[ ! -d "$homeDir" ]]; then
        requirementFulfilled=1
    else
        owner=$(stat -c '%u' "$homeDir")

        if [[ "$userID" != "$owner" ]]; then
            requirementFulfilled=1
        elif [[ ! -w "$homeDir" ]]; then
            requirementFulfilled=1
        fi
    fi

    if [[ "$requirementFulfilled" -eq 1 ]]; then
        totalNotRootRSS=0
        pids=$(mktemp)

        while read -r user uid pid rss; do
            if [[ "$userID" == "$uid" ]]; then
                totalNotRootRSS=$((totalNotRootRSS + rss))
                echo "$pid" >> "$pids"
            fi
        done < "$notRootOnly"

        if [[ "$totalNotRootRSS" -gt "$totalRSSroot" ]]; then
            while read -r pid; do
                kill -TERM "$pid"
            done < "$pids"
        fi

        rm "$pids"
    fi

done < "$notRootUsers"

rm "$all" "$rootOnly" "$notRootOnly" "$notRootUsers"
#######################################################################################################


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
