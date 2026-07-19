#!/bin/bash

localFile="/etc/passwd"

if [[ -n "$PASSWD" ]]; then
    localFile="$PASSWD"
fi

# директорията, в която е скриптът (за да намерим occ)
scriptDir="$(dirname "$0")"

# локални потребители (UID >= 1000)
users=$(mktemp)

awk -F: '$3 >= 1000 {print $1}' "$localFile" > "$users"


# PrevCloud потребители
userInCloud=$(mktemp)

"$scriptDir/occ" user:list > "$userInCloud"


# Синхронизация локални -> PrevCloud
while read -r user; do

    if ! grep -Eq "^- $user:" "$userInCloud"; then
        # няма такъв потребител в PrevCloud
        "$scriptDir/occ" user:add "$user"

    else
        # има потребител, проверяваме дали е заключен
        enabled=$("$scriptDir/occ" user:info "$user" | grep "enabled:" | cut -d ':' -f2 | tr -d ' ')

        if [[ "$enabled" == "false" ]]; then
            "$scriptDir/occ" user:enable "$user"
        fi
    fi

done < "$users"



# Синхронизация PrevCloud -> локални
while read -r line; do

    user=$(echo "$line" | cut -d ':' -f1 | sed 's/ - //')

    if ! grep -Eq "^$user$" "$users"; then

        enabled=$("$scriptDir/occ" user:info "$user" | grep "enabled:" | cut -d ':' -f2 | tr -d ' ')

        if [[ "$enabled" == "true" ]]; then
            "$scriptDir/occ" user:disable "$user"
        fi

    fi

done < "$userInCloud"


rm "$users" "$userInCloud"
-------------------------------------------------------------------------------------------------
#!/bin/bash

if [[ -z "$PASSWD" ]]; then
    passwd_file="/etc/passwd"
else
    passwd_file="$PASSWD"
fi

script_dir=$(dirname "$0")
occ="$script_dir/occ"

local_users=$(mktemp)
cloud_users=$(mktemp)

# взимаме локалните потребители
cat "$passwd_file" | while IFS=':' read username pass uid gid gecos home shell; do
    if [[ "$uid" -ge 1000 ]]; then
        echo "$username"
    fi
done | sort > "$local_users"

# взимаме PrevCloud потребителите
"$occ" user:list | while read line; do
    user=$(echo "$line" | cut -d ':' -f1 | sed 's/- //')
    echo "$user"
done | sort > "$cloud_users"

# локален потребител, който го няма в cloud
cat "$local_users" | while read user; do

    found=$(grep "^$user$" "$cloud_users")

    if [[ -z "$found" ]]; then
        "$occ" user:add "$user"
    else

        enabled=$("$occ" user:info "$user" | grep "enabled:" | cut -d ' ' -f2)

        if [[ "$enabled" == "false" ]]; then
            "$occ" user:enable "$user"
        fi
    fi

done

# cloud потребител, който го няма локално
cat "$cloud_users" | while read user; do

    found=$(grep "^$user$" "$local_users")

    if [[ -z "$found" ]]; then

        enabled=$("$occ" user:info "$user" | grep "enabled:" | cut -d ' ' -f2)

        if [[ "$enabled" == "true" ]]; then
            "$occ" user:disable "$user"
        fi
    fi

done

rm "$local_users"
rm "$cloud_users"
