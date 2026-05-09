#!/bin/bash

if [[ $# -lt 1 ]]; then
    echo "ERROR:no files given" >&2
    exit 1
fi

today=$(date +%Y%m%d)

for file in "$@"; do

    if [[ ! -f "$file" ]]; then
        echo "ERROR:$file:not a file" >&2
        continue
    fi

    if [[ ! -r "$file" || ! -w "$file" ]]; then
        echo "ERROR:$file:cannot read or write" >&2
        continue
    fi

    serial=$(
        sed 's/;.*//' "$file" |
        tr '\n' ' ' |
        tr -s '[:space:]' ' ' |
        grep -Eo '[0-9]{10}' |
        head -n 1
    )

    if [[ -z "$serial" ]]; then
        echo "ERROR:$file:cannot find serial" >&2
        continue
    fi

    old_date=$(echo "$serial" | cut -c 1-8)
    old_tt=$(echo "$serial" | cut -c 9-10)

    if [[ "$old_date" < "$today" ]]; then

        new_serial="${today}00"

    elif [[ "$old_date" == "$today" ]]; then

        if [[ "$old_tt" == "99" ]]; then
            echo "ERROR:$file:serial limit reached" >&2
            continue
        fi

        new_tt=$((old_tt + 1))

        if [[ $new_tt -le 9 ]]; then
            new_tt="0$new_tt"
        fi

        new_serial="$today$new_tt"

    else
        echo "ERROR:$file:serial date is in future" >&2
        continue
    fi

    sed -i "s/$serial/$new_serial/" "$file"

done
