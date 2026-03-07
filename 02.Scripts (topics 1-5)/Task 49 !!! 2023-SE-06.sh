#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "error"
    exit 1
fi

if [[ ! -d "$1" ]]; then
    echo "Camera directory does not exist"
    exit 1
fi

if [[ -e "$2" ]]; then
    echo "Library already exists"
    exit 1
fi

mkdir "$2"

photos=$(mktemp)
days=$(mktemp)
mapFile=$(mktemp)

while IFS= read-r -d ' ' photo;do
  picTime=$(stat -c '%y' "$photo")
  picDate=$(echo "$picTime" |  cut -d ' ' -f1)
  picNewName=$(echo "$picTime" | cut -d '.' -f1 | tr ' ' '_')
  echo "${picDate}|${picNewName}|${photo}" >> "$photos"
done < <(find "$1" -type f -name "*.jpg" -print0)

sort -t '|' -k1,1 "$photos" -o "$photos"
cut -d '|' -f 1 "$photos" | uniq > "$days"

startPeriod=$(head -n1 "$days")
endPeriod=$(head -n1 "$days")

while read -r currDate;do
  if [[ "$currDate" == "$endPeriod" ]];then
    continue;
  fi
  nextDay=$(date -d "$endPeriod +1 day" + '%Y-%m-%d')
  if [[ "$currDate" != "$nextDay" ]];then
    interval=${startPeriod}_${endPeriod}
    curr=$startPeriod

    while true;do
          echo "${curr}|${interval}" >> $mapFile
          if [[ "$curr" =="$endPeriod"]];then
            break;
            fi
             curr=$(date -d "$curr +1 day" + '%Y-%m-%d')
      done
      mkdir "$2/$interval"
      startPeriod=$currDate;
    fi
    endPeriod=$currDate
done< "$days"
if [[ -n "$startPeriod" ]]; then
    interval="${startPeriod}_${endPeriod}"
    curr="$startPeriod"

    while true; do
        echo "${curr}|${interval}" >> "$mapFile"

        if [[ "$curr" == "$endPeriod" ]]; then
            break
        fi

        curr=$(date -d "$curr + 1 day" +'%Y-%m-%d')
    done

    if [[ ! -d "$2/$interval" ]]; then
        mkdir "$2/$interval"
    fi
fi

while IFS='|' read -r currPhotoDate currPhotoName currPhotoPath; do
    interval=$(grep "^${currPhotoDate}|" "$mapFile" | head -n 1 | cut -d '|' -f 2)
    cp "$currPhotoPath" "$2/$interval/$currPhotoName.jpg"
done < "$photos"
