#!/bin/bash

if [[ ${#} -ne 0 ]];
then
    echo "error"
    exit 1
fi

# passwd файл
if [[ -n "$PASSWD" ]];
then
    passwdFile="$PASSWD"
else
    passwdFile="/etc/passwd"
fi

SCRIPT_DIR=$(dirname "$0")   #програмата occ се намира в същата директория като скрипта
users=$(
    awk -F: '$3 >= 1000 { print $1 }' "$passwdFile" | sort
)
localsNotInPC=$(comm -23 <(echo "$users") <(echo "$prevCloudUsers"))
PCNotInLocal=$(comm -13 <(echo "$users") <(echo "$prevCloudUsers"))
mutual=$(comm -12 <(echo "$users") <(echo "$prevCloudUsers"))

while read -r user;
do
    if [[ -n "$user" ]];
    then
        "$SCRIPT_DIR"/occ user:add "$user"
    fi
done < <(echo "$localsNotInPC")

while read -r user;
do
    if [[ -n "$user" ]];
    then
        info=$(
            "$SCRIPT_DIR"/occ user:info "$user" |
            grep "enabled:" |
            cut -d: -f2 |
            tr -d ' '
        )

        if [[ "$info" == "false" ]];
        then
            "$SCRIPT_DIR"/occ user:enable "$user"
        fi
    fi
done < <(echo "$mutual")

while read -r user;
do
    if [[ -n "$user" ]];
    then
        info=$(
            "$SCRIPT_DIR"/occ user:info "$user" |
            grep "enabled:" |
            cut -d: -f2 |
            tr -d ' '
        )

        if [[ "$info" == "true" ]];
        then
            "$SCRIPT_DIR"/occ user:disable "$user"
        fi
    fi
done < <(echo "$PCNotInLocal")

exit 0
