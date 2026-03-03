#!/bin/bash

if [[ $# -ne 1 ]];then
  exit 1
fi

config="${1}"
if [[ ! -f "$config" ]];then
  exit  2 ;
fi
if [[ ! -r "$config" ]];then
   echo "not readable"
   exit 3
fi
if [[ ! -w "$config" ]];then
   echo "not writable"
   exit 3
fi

cat "$config" | sed -E 's/[[:space:]]+/ /g'
found=0
file="/proc/acpi/wakeup"
cat "$file" | sed -E 's/[[:space:]]+/ /g'

while IFS= read -r line; do
  # махаме коментарите и празните редове (иначе ще чупят парсването)
  line="${line%%#*}"
  line="$(echo "$line" | sed -E 's/^[[:space:]]+//; s/[[:space:]]+$//')"
  [[ -z "$line" ]] && continue

  # вместо cut -d '' (невъзможно): взимаме 1-во и 2-ро поле от текущия ред
  read -r code status _rest <<< "$line"

  # ако няма и двете полета -> прескачаме
  [[ -z "${code:-}" || -z "${status:-}" ]] && continue

  # 1) проверка дали устройството го има във /proc/acpi/wakeup
  if grep -qE "^[[:space:]]*${code}[[:space:]]" "$file"; then
    found=1

    # 2) взимаме текущия status за точно това устройство (3-та колона), махаме '*'
    statusHere="$(
      grep -E "^[[:space:]]*${code}[[:space:]]" "$file" \
        | head -n 1 \
        | tr -s '[:space:]' ' ' \
        | cut -d ' ' -f3 \
        | tr -d '*'
    )"

    # 3) ако е същото -> нищо
    if [[ "$status" == "$statusHere" ]]; then
      continue
    else
      # 4) ако е различно -> toggle чрез записване на кода
      echo "$code" > "$file"
    fi
  else
    # ако не съществува устройството - предупреждение (удобен инструмент)
    echo "Warning: device '$code' not found" >&2
  fi

done < "$config"
