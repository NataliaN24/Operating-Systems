#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <config-file>" >&2
    exit 1
fi

config="$1"
file="/proc/acpi/wakeup"

if [[ ! -f "$config" ]]; then
    echo "Error: config file does not exist" >&2
    exit 2
fi

if [[ ! -r "$config" ]]; then
    echo "Error: config file is not readable" >&2
    exit 3
fi

if [[ ! -r "$file" || ! -w "$file" ]]; then
    echo "Error: cannot read/write $file. Run as root." >&2
    exit 4
fi

while read -r line; do
    clean=$(echo "$line" | sed 's/#.*//' | tr -s '[:space:]' ' ' | sed 's/^ //; s/ $//')

    if [[ -z "$clean" ]]; then
        continue
    fi

    device=$(echo "$clean" | cut -d ' ' -f1)
    wanted=$(echo "$clean" | cut -d ' ' -f2)

    if [[ ! "$wanted" =~ ^(enabled|disabled)$ ]]; then
        echo "Warning: invalid state for device $device: $wanted" >&2
        continue
    fi

    current_line=$(grep -E "^$device[[:space:]]" "$file")

    if [[ -z "$current_line" ]]; then
        echo "Warning: device $device does not exist" >&2
        continue
    fi

    current=$(echo "$current_line" | tr -s '[:space:]' ' ' | cut -d ' ' -f3 | sed 's/^\*//')

    if [[ "$current" != "$wanted" ]]; then
        echo "$device" > "$file"
    fi

done < "$config"
##################################################################################################################




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
