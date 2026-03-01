#!/bin/bash

if [[ $# -ne 1]];then
  exit 1
fi

device=${1}
file="/proc/acpi/wakeup"

if [[ ! "$device" =~ ^[A-Z0-9]{1,4}$ ]];then
  exit 1
fi

row=$(grep "^$device[[:space:]]" "$file")

if [[ -z "$row"]];then
  exit 2;
fi

status=$(echo "$row" | awk '{print $3}')
if [[ "$status" == "*disabled" ]];then
  exit 0
fi
echo $device" > "$file"
