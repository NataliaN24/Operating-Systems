#!/bin/bash

dir="$1"

# Проверка за директория
if [[ ! -d "$dir" ]]; then
    exit 1
fi

candidates=$(mktemp)
data=$(mktemp)
files=$(mktemp)

# Общо количество файлове
totalFiles=$(find "$dir" -type f | wc -l)

# За всеки файл:
# word count
# Например: you 5
find "$dir" -type f | while read -r file; do
    grep -Eo '[a-z]+' "$file" |
    sort |
    uniq -c |
    while read -r count word; do
        echo "$word $count" >> "$data"
    done
done

# Списък с всички различни думи
cut -d ' ' -f1 "$data" | sort -u > "$files"

# За всяка дума
while read -r word; do

    total=0
    fileCount=0

    # Намираме всички срещания на тази дума
    grep "^$word " "$data" |
    while read -r w count; do
        echo "$count"
    done > "$candidates"

    # Общо срещания
    while read -r count; do
        total=$((total + count))
    done < "$candidates"

    # В колко файла думата се среща поне 3 пъти
    # Тук обаче data не пази отделните файлове,
    # затова трябва да се направи отново проверка по файлове.

    fileCount=0

    find "$dir" -type f | while read -r file; do
        count=$(grep -Eo '[a-z]+' "$file" | grep "^$word$" | wc -l)

        if [[ "$count" -ge 3 ]]; then
            echo 1
        fi
    done > "$candidates"

    fileCount=$(wc -l < "$candidates")

    # Проверка:
    # fileCount >= totalFiles / 2
    half=$((totalFiles / 2))

    if [[ "$fileCount" -ge "$half" ]]; then
        echo "$total $word"
    fi

done < "$files" |
sort -nr |
head -10

rm -f "$candidates" "$data" "$files"

/////////////////////////////////////////
#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

if [[ ! -d "$1" ]]; then
    exit 2
fi

dir="$1"

tmp_words=$(mktemp)
tmp_file_words=$(mktemp)
tmp_total=$(mktemp)
tmp_file_counts=$(mktemp)

find "$dir" -type f | while read -r file; do
    grep -o '[a-z]\+' "$file" >> "$tmp_words"

    grep -o '[a-z]\+' "$file" | sort | uniq -c | while read -r cnt word; do
        if [[ "$cnt" -ge 3 ]]; then
            echo "$word" >> "$tmp_file_words"
        fi
    done
done

files_count=$(find "$dir" -type f | wc -l)

if [[ "$files_count" -eq 0 ]]; then
    rm "$tmp_words" "$tmp_file_words" "$tmp_total" "$tmp_file_counts"
    exit 0
fi

sort "$tmp_words" | uniq -c > "$tmp_total"
sort "$tmp_file_words" | uniq -c > "$tmp_file_counts"

while read -r file_cnt word; do
    if [[ $((file_cnt * 2)) -ge "$files_count" ]]; then
        total_cnt=$(grep " $word$" "$tmp_total" | tr -s ' ' | cut -d ' ' -f2)
        echo "$total_cnt $word"
    fi
done < "$tmp_file_counts" | sort -nr | head -n 10 | cut -d ' ' -f2

rm "$tmp_words" "$tmp_file_words" "$tmp_total" "$tmp_file_counts"
#########################################################################################


#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <dir>" >&2
    exit 1
fi

dir="$1"

if [[ ! -d "$dir" ]]; then
    echo "Invalid directory" >&2
    exit 1
fi

files_count=$(find "$dir" -type f | wc -l)

if [[ "$files_count" -eq 0 ]]; then
    exit 0
fi

needed=$(( (files_count + 1) / 2 ))

tmp=$(mktemp)

trap 'rm -f "$tmp"' EXIT

find "$dir" -type f | while read -r file; do

    tr -c 'a-z' '\n' < "$file" |
    grep -v '^$' |
    sort |
    uniq -c |

    while read -r count word; do
        echo "$word $count $file"
    done

done > "$tmp"

cut -d ' ' -f1 "$tmp" | sort | uniq |

while read -r word; do

    total=0

    while read -r count; do
        total=$(( total + count ))
    done < <(
        grep "^$word " "$tmp" |
        cut -d ' ' -f2
    )

    good_files=0

    while read -r count; do

        if [[ "$count" -ge 3 ]]; then
            good_files=$(( good_files + 1 ))
        fi

    done < <(
        grep "^$word " "$tmp" |
        cut -d ' ' -f2
    )

    if [[ "$good_files" -ge "$needed" ]]; then
        echo "$total $word"
    fi

done |

sort -nr |
head -n 10 |
cut -d ' ' -f2
