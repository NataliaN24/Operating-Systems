#!/bin/bash

if [[ -z "$CTRLSLOTS" ]]; then
    slots="0"
else
    slots="$CTRLSLOTS"
fi

if [[ $# -gt 1 ]]; then
    echo "Usage: $0 [autoconf|config]" >&2
    exit 1
fi

if [[ $# -eq 1 && "$1" == "autoconf" ]]; then
    echo "yes"
    exit 0
fi

mode="values"

if [[ $# -eq 1 && "$1" == "config" ]]; then
    mode="config"

    echo "graph_title SSA drive temperatures"
    echo "graph_vlabel Celsius"
    echo "graph_category sensors"
    echo "graph_info This graph shows SSA drive temp"

elif [[ $# -eq 1 ]]; then
    echo "Usage: $0 [autoconf|config]" >&2
    exit 1
fi

for slot in $slots; do

    model=""
    array=""
    drive=""

    ssacli ctrl slot="$slot" pd all show detail | while read -r line; do

        if echo "$line" | grep -q '^Smart Array '; then

            model=$(echo "$line" | cut -d ' ' -f3)

        elif echo "$line" | grep -q '^Array '; then

            array=$(echo "$line" | cut -d ' ' -f2)

        elif echo "$line" | grep -q '^Unassigned$'; then

            array="UN"

        elif echo "$line" | grep -q '^physicaldrive '; then

            drive=$(echo "$line" | cut -d ' ' -f2)

        elif echo "$line" | grep -q '^Current Temperature (C):'; then

            temp=$(echo "$line" | cut -d ':' -f2 | tr -d ' ')

            drive_id=$(echo "$drive" | tr -d ':')

            id="SSA${slot}${model}${array}${drive_id}"

            label="SSA${slot} ${model} ${array} ${drive}"

            if [[ "$mode" == "config" ]]; then

                echo "$id.label $label"
                echo "$id.type GAUGE"

            else

                echo "$id.value $temp"

            fi
        fi

    done

done
