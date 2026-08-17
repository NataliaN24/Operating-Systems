#!/bin/bash

if [[ $# -ne 2 ]]; then
    exit 1
fi

dir="$1"
if [[ ! -d "$dir" ]]; then
    exit 2
fi

hashfile="$2"
if [[ ! -f "$hashfile" ]]; then
    exit 3
fi

tmp=$(mktemp)
allFiles=$(mktemp)
sameDateFiles=$(mktemp)
history=$(mktemp)
filesToDelete=$(mktemp)
processedDates=$(mktemp)

# Get all data files
find "$dir" -type f -name 'Forecast_*.parquet' > "$allFiles"

# Add line numbers to the history file
line=0
while read -r hash; do
    line=$((line + 1))
    echo "$hash $line" >> "$history"
done < "$hashfile"


while read -r file; do

    # Extract date from filename
    rawDate=$(basename "$file" | grep -Eo '[0-9]+\.[0-9]+\.[0-9]{4}')

    # Normalize dates:
    # 5.7.2025, 05.7.2025 and 5.07.2025 -> 2025-07-05
    day=$(echo "$rawDate" | cut -d '.' -f1)
    month=$(echo "$rawDate" | cut -d '.' -f2)
    year=$(echo "$rawDate" | cut -d '.' -f3)

    date=$(printf "%04d-%02d-%02d" "$year" "$month" "$day")

    # If we already processed this date, skip it
    if grep -Fxq "$date" "$processedDates"; then
        continue
    fi

    echo "$date" >> "$processedDates"

    # Find all files having the same date
    > "$sameDateFiles"

    while read -r f; do
        rawDate2=$(basename "$f" | grep -Eo '[0-9]+\.[0-9]+\.[0-9]{4}')

        day2=$(echo "$rawDate2" | cut -d '.' -f1)
        month2=$(echo "$rawDate2" | cut -d '.' -f2)
        year2=$(echo "$rawDate2" | cut -d '.' -f3)

        date2=$(printf "%04d-%02d-%02d" "$year2" "$month2" "$day2")

        if [[ "$date" == "$date2" ]]; then
            echo "$f" >> "$sameDateFiles"
        fi

    done < "$allFiles"


    # Find the newest file for this date
    cnt=0
    fileToKeep=""

    while read -r f; do

        # The task uses SHA-1, NOT SHA-256
        hash=$(sha1sum "$f" | cut -d ' ' -f1)

        # Find this hash in the history file
        historyLine=$(grep -E "^$hash " "$history")

        if [[ -z "$historyLine" ]]; then
            # Hash not found in history.
            # Such a file cannot be ordered according to the task.
            continue
        fi

        number=$(echo "$historyLine" | cut -d ' ' -f2)

        if [[ "$number" -gt "$cnt" ]]; then
            cnt="$number"
            fileToKeep="$f"
        fi

    done < "$sameDateFiles"


    # Delete all files except the newest one
    while read -r f; do
        if [[ "$f" != "$fileToKeep" ]]; then
            echo "Deleting: $f"
            rm -- "$f"
        fi
    done < "$sameDateFiles"

done < "$allFiles"
/////////////////////////////////////////////////////////////////////////////////////
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
