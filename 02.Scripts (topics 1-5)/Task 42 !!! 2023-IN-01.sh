#!/bin/bash

# -------- 1) Проверка аргументи / режим --------
if [[ $# -eq 1 && "$1" == "autoconf" ]]; then
  echo "yes"
  exit 0
fi

mode="values"
if [[ $# -eq 1 && "$1" == "config" ]]; then
  mode="config"
elif [[ $# -eq 0 ]]; then
  mode="values"
elif [[ $# -ne 0 ]]; then
  exit 1
fi

# -------- 2) Кои слотове ще обхождаме --------
# По подразбиране слот 0, ако няма CTRLSLOTS
if [[ -z "${CTRLSLOTS:-}" ]]; then
  slots="0"
else
  slots="$CTRLSLOTS"
fi

# -------- 3) Ако е config -> header-и --------
if [[ "$mode" == "config" ]]; then
  echo "graph_title SSA drive temperatures"
  echo "graph_vlabel Celsius"
  echo "graph_category sensors"
  echo "graph_info This graph shows SSA drive temp"
fi

# -------- 4) Основна част: обхождаме слотовете --------
for slot in $slots; do
  model=""
  array=""
  drive=""

  # Четем изхода на ssacli за дадения слот
  ssacli ctrl slot="$slot" pd all show detail 2>/dev/null |
  while IFS= read -r line; do
    case "$line" in
      # 4.1) Ред за контролер: "Smart Array P420i in Slot 0 ..."
      "Smart Array "*)
        # взимаме 3-тото поле: P420i / P822 ...
        model=$(echo "$line" | cut -d ' ' -f3)
        ;;

      # 4.2) Секция масив: "Array A"
      "Array "*)
        # взимаме 2-рото поле: A / J ...
        array=$(echo "$line" | cut -d ' ' -f2)
        ;;

      # 4.3) Секция Unassigned -> за задачата е UN
      "Unassigned")
        array="UN"
        ;;

      # 4.4) Подсекция диск: "physicaldrive 1I:2:1"
      "physicaldrive "*)
        # взимаме 2-рото поле: 1I:2:1 / 2E:1:22 ...
        drive=$(echo "$line" | cut -d ' ' -f2)
        ;;

      # 4.5) Температура: "Current Temperature (C): 35"
      "Current Temperature (C): "*)
        # взимаме последното поле -> числото
        temp=$(echo "$line" | awk '{print $NF}')

        # ако имаме контекст (model/array/drive), печатаме
        if [[ -n "$model" && -n "$array" && -n "$drive" ]]; then
          ppp=$(echo "$drive" | tr -d ':')
          id="SSA${slot}${model}${array}${ppp}"
          label="SSA${slot} ${model} ${array} ${drive}"

          if [[ "$mode" == "config" ]]; then
            echo "${id}.label ${label}"
            echo "${id}.type GAUGE"
          else
            echo "${id}.value ${temp}"
          fi
        fi
        ;;
    esac
  done
done
