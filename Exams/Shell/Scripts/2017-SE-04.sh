#Напишете shell script, който получава задължителен първи позиционен параметър
#– директория и незадължителен втори – име на файл. Скриптът трябва да намира в подадената
#директория и нейните под-директории всички symlink-ове и да извежда (при подаден аргумент
#файл – добавяйки към файла, а ако не е – на стандартния изход) за тях следната информация:
#• ако destination-a съществува – името на symlink-а -> името на destination-а;
#• броя на symlink-овете, чийто destination не съществува.
#Примерен изход:
#8
#lbaz -> /foo/bar/baz
#lqux -> ../../../qux
#lquux -> /foo/quux
#Broken symlinks: 34

#!/bin/bash

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <directory> [output_file]"
    exit 1
fi

DIR="$1"
OUTPUT="${2:-}"  # ако има втори аргумент, записваме във файла, иначе stdout

if [[ ! -d "$DIR" ]]; then
    echo "Not a directory"
    exit 2
fi

broken_count=0

find "$DIR" -type l | while read link; do
    target=$(readlink "$link")
    if [[ -e "$link" ]]; then
        if [[ -n "$OUTPUT" ]]; then
            echo "$link -> $target" >> "$OUTPUT"
        else
            echo "$link -> $target"
        fi
    else
        ((broken_count++))
    fi
done

if [[ -n "$OUTPUT" ]]; then
    echo "Broken symlinks: $broken_count" >> "$OUTPUT"
else
    echo "Broken symlinks: $broken_count"
fi
