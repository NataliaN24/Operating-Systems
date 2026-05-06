#!/bin/bash

if [[ $# -ne 3 ]]; then
    echo "Usage: $0 config key value"
    exit 1
fi

file="$1"
key="$2"
new_value="$3"

if [[ ! -f "$file" ]]; then
    echo "Invalid file"
    exit 1
fi

date_now=$(date)
user_now=$(whoami)

tmp=$(mktemp)
found=0

while read -r line; do

    # махаме коментара само за проверката
    clean=$(echo "$line" | cut -d '#' -f1)

    # търсим дали този ред е key = value
    if echo "$clean" | grep -Eq "^[[:space:]]*$key[[:space:]]*="; then

        found=1

        old_value=$(echo "$clean" |
            cut -d '=' -f2 |
            tr -d '[:space:]')

        if [[ "$old_value" != "$new_value" ]]; then
            echo "# $line # edited at $date_now by $user_now" >> "$tmp"
            echo "$key = $new_value # added at $date_now by $user_now" >> "$tmp"
        else
            echo "$line" >> "$tmp"
        fi

    else
        echo "$line" >> "$tmp"
    fi

done < "$file"

if [[ $found -eq 0 ]]; then
    echo "$key = $new_value # added at $date_now by $user_now" >> "$tmp"
fi

mv "$tmp" "$file"
