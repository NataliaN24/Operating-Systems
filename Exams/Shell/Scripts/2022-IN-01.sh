Както знаете, при отваряне на файл с редактора vi, той създава в същата директория временен файл
с име в следния формат: точка, името на оригиналния файл, точка, swp. Например, при редактиране
на файл с име foo.txt ще се създаде временен файл с име .foo.txt.swp.
Напишете shell скрипт, който приема два задължителни позиционни аргумента – имена на директории. Примерно извикване: ./foo.sh ./dir1 /path/to/dir2/
В dir1 може да има файлове/директории, директорията dir2 трябва да е празна.
Скриптът трябва да копира всички обикновенни файлове от dir1 (и нейните под-директории) в dir2,
запазвайки директорийната структура, но без да копира временните файлове, създадени от редактора
vi (по горната дефиниция).
Забележка: За удобство приемаме, че не ни вълнува дали метаданните на обектите (owner, group,
permissions, etc.) ще се запазят.
Примерни обекти:
dir1/
dir1/a
dir1/.a.swp
dir1/b
dir1/c/d
dir1/c/.bar.swp
Обекти след изпълнението:
dir2/
dir2/a
dir2/b
dir2/c/d
dir2/c/.bar.swp

#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 <source_dir> <dest_dir>"
    exit 1
fi

SRC="$1"
DST="$2"

if [[ ! -d "$DST" ]]; then
    echo "Destination directory does not exist"
    exit 1
fi

if [[ $(ls -A "$DST") ]]; then
    echo "Destination directory must be empty"
    exit 1
fi

cd "$SRC" || exit 1

find . -type f ! -name ".*.swp" | while read -r file; do
    mkdir -p "$DST/$(dirname "$file")"
    cp "$file" "$DST/$file"
done

