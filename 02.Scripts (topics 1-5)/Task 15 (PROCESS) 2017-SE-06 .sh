#!/bin/bash

user=$(whoami)

if [[ "$user" != "root" ]];then
    exit 1
fi

nonRoot=$(mktemp)

cat /etc/passwd | while read -r line;do
    criteriaFulfilled=0

    user=$(echo "$line" | cut -d ':' -f1)
    id=$(echo "$line" | cut -d ':' -f3)
    home=$(echo "$line" | cut -d ':' -f6)

    if [[ "$user" == "root" ]];then
        continue
    fi

    if [[ ! -d "$home" ]];then
        criteriaFulfilled=1
    else
        homeOwner=$(stat -c '%u' "$home")
        write=$(stat -c '%A' "$home" | cut -c3)

        if [[ "$id" != "$homeOwner" ]];then
            criteriaFulfilled=1
        elif [[ "$write" != "w" ]];then
            criteriaFulfilled=1
        fi
    fi

    if [[ "$criteriaFulfilled" -eq 1 ]];then
        echo "$user" >> "$nonRoot"
    fi
done


rootProcesses=$(mktemp)
rootTotal=0

ps -eo user=,rss= | grep "^root " > "$rootProcesses"

while read -r user rss;do
    rootTotal=$((rootTotal + rss))
done < "$rootProcesses"


tempRes=$(mktemp)

ps -eo user=,pid=,rss= > "$tempRes"

userTotals=$(mktemp)

while read -r user pid rss;do

    found=$(grep -x "$user" "$nonRoot")

    if [[ -z "$found" ]];then
        continue
    fi

    old=$(grep "^$user " "$userTotals" | cut -d ' ' -f2)

    if [[ -z "$old" ]];then
        echo "$user $rss" >> "$userTotals"
    else
        totalRss=$((old + rss))

        sed -i "s/^$user .*/$user $totalRss/" "$userTotals"
    fi

done < "$tempRes"


while read -r user totalRss;do

    if [[ "$totalRss" -gt "$rootTotal" ]];then

        grep "^$user " "$tempRes" | while read -r u pid rss;do
            kill "$pid"
        done

    fi

done < "$userTotals"


rm "$nonRoot"
rm "$rootProcesses"
rm "$tempRes"
rm "$userTotals"
//////////////////////////////////////
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
