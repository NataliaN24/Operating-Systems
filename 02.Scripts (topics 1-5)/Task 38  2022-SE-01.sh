#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <config_file>" >&2
    exit 1
fi

config="$1"
file="/proc/acpi/wakeup"

if [[ ! -f "$config" ]]; then
    echo "Error: config file does not exist" >&2
    exit 1
fi

if [[ ! -r "$file" || ! -w "$file" ]]; then
    echo "Error: cannot access $file" >&2
    exit 1
fi

cat "$config" |
sed 's/#.*//' |
while read -r device state; do

    [[ -z "$device" ]] && continue

    if ! grep -q "^$device[[:space:]]" "$file"; then
        echo "Warning: device $device does not exist" >&2
        continue
    fi

    currentState=$(grep "^$device[[:space:]]" "$file" | awk '{print $3}')

    currentState="${currentState#\*}"

    if [[ "$currentState" != "$state" ]]; then
        echo "$device" > "$file"
    fi

done
