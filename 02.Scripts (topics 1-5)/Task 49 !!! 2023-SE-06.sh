#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 CAMERA_DIR LIBRARY_DIR"
    exit 1
fi

camera="$1"
library="$2"

if [[ ! -d "$camera" ]]; then
    echo "Camera directory does not exist"
    exit 1
fi

if [[ -e "$library" ]]; then
    echo "Library directory already exists"
    exit 1
fi

mkdir "$library"

photos=$(mktemp)
dates=$(mktemp)
map=$(mktemp)

find "$camera" -type f -name "*.jpg" | while read -r file; do
    ts=$(stat -c '%Y' "$file")

    day=$(date -d "@$ts" +'%Y-%m-%d')

    newname=$(date -d "@$ts" +'%Y-%m-%d_%H:%M:%S.jpg')

    echo "$ts $day $newname $file"

done | sort -n > "$photos"

cut -d ' ' -f2 "$photos" | sort -u > "$dates"

start=""
prev=""

while read -r day; do

    if [[ "$start" == "" ]]; then

        start="$day"
        prev="$day"

    else

        next=$(date -d "$prev + 1 day" +'%Y-%m-%d')

        if [[ "$day" == "$prev" ]]; then

            continue

        elif [[ "$day" == "$next" ]]; then

            prev="$day"

        else

            interval="${start}_${prev}"

            d="$start"

            while [[ "$d" < "$prev" || "$d" == "$prev" ]]; do

                echo "$d $interval" >> "$map"

                d=$(date -d "$d + 1 day" +'%Y-%m-%d')

            done

            start="$day"
            prev="$day"

        fi
    fi

done < "$dates"

if [[ "$start" != "" ]]; then

    interval="${start}_${prev}"

    d="$start"

    while [[ "$d" < "$prev" || "$d" == "$prev" ]]; do

        echo "$d $interval" >> "$map"

        d=$(date -d "$d + 1 day" +'%Y-%m-%d')

    done
fi

while read -r ts day newname file; do

    interval=$(grep "^$day " "$map" | cut -d ' ' -f2)

    mkdir -p "$library/$interval"

    cp "$file" "$library/$interval/$newname"

done < "$photos"

rm -f "$photos" "$dates" "$map"
#########################################################################################
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
