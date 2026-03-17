
#!/bin/bash

if [[ $# -ne 2 ]]; then
    exit 1
fi

dataDir="$1"
historyFile="$2"

if [[ ! -d "$dataDir" ]]; then
    exit 2
fi

if [[ ! -f "$historyFile" ]]; then
    exit 3
fi

allInfo=$(mktemp)
latestInfo=$(mktemp)

# allInfo ще пази редове:
# hash YYYY-MM-DD /path/to/file

find "$dataDir" -maxdepth 1 -type f -name 'Forecast_*.parquet' | while read -r file; do
    base=$(basename "$file")

    if echo "$base" | grep -Eq '^Forecast_[0-9]+\.[0-9]+\.[0-9]+_[0-9]+\.parquet$'; then
        datePart=$(echo "$base" | cut -d '_' -f2)

        day=$(echo "$datePart" | cut -d '.' -f1)
        month=$(echo "$datePart" | cut -d '.' -f2)
        year=$(echo "$datePart" | cut -d '.' -f3)

        day=$((10#$day))
        month=$((10#$month))

        normDate=$(printf "%04d-%02d-%02d" "$year" "$month" "$day")
        hash=$(sha1sum "$file" | cut -d ' ' -f1)

        echo "$hash $normDate $file" >> "$allInfo"
    fi
done

# latestInfo ще пази редове:
# YYYY-MM-DD /path/to/file
# винаги последния срещнат файл за датата

while read -r hash; do
    found=$(grep "^$hash " "$allInfo")

    if [[ -n "$found" ]]; then
        date=$(echo "$found" | cut -d ' ' -f2)
        file=$(echo "$found" | cut -d ' ' -f3-)

        grep -v "^$date " "$latestInfo" > "${latestInfo}.tmp"
        mv "${latestInfo}.tmp" "$latestInfo"

        echo "$date $file" >> "$latestInfo"
    fi
done < "$historyFile"

find "$dataDir" -maxdepth 1 -type f -name 'Forecast_*.parquet' | while read -r file; do
    base=$(basename "$file")

    if echo "$base" | grep -Eq '^Forecast_[0-9]+\.[0-9]+\.[0-9]+_[0-9]+\.parquet$'; then
        datePart=$(echo "$base" | cut -d '_' -f2)

        day=$(echo "$datePart" | cut -d '.' -f1)
        month=$(echo "$datePart" | cut -d '.' -f2)
        year=$(echo "$datePart" | cut -d '.' -f3)

        day=$((10#$day))
        month=$((10#$month))

        normDate=$(printf "%04d-%02d-%02d" "$year" "$month" "$day")

        latestFile=$(grep "^$normDate " "$latestInfo" | cut -d ' ' -f2-)

        if [[ "$file" != "$latestFile" ]]; then
            rm -f -- "$file"
        fi
    fi
done

rm -f "$allInfo" "$latestInfo"
