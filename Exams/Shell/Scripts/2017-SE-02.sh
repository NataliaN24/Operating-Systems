#Напишете скрипт, който приема три задължителни позиционни параметра - директория SRC, директория DST
#(която не трябва да съдържа файлове) и низ АBC. Ако скриптът се
#изпълнява от root потребителя, то той трябва да намира всички файлове в директорията SRC и
#нейните под-директории, които имат в името си като под-низ АBC, и да ги мести в директорията
#DST, запазвайки директорийната структура (но без да запазва мета-данни като собственик и права,
#т.е. не ни интересуват тези параметри на новите директории, които скриптът би генерирал в DST).
#Пример:
#• в SRC (/src) има следните файлове:
#/src/foof.txt
#/src/1/bar.txt
#/src/1/foo.txt
#/src/2/1/foobar.txt
#/src/2/3/barf.txt
#• DST (/dst) е празна директория
#• зададения низ е foo
#Резултат:
#• в SRC има следните файлове:
#/src/1/bar.txt
#/src/2/3/barf.txt
#• в DST има следните файлове:
#/dst/foof.txt
#/dst/1/foo.txt
#/dst/2/1/foobar.txt

#!/bin/bash
if [[ $# -ne 3 ]]; then
    echo "Usage: $0 <SRC> <DST> <substring>"
    exit 1
fi

SRC="$1"
DST="$2"
SUB="$3"
if [[ $EUID -ne 0 ]]; then
    echo "This script must be run as root."
    exit 2
fi
if [[ ! -d "$SRC" ]]; then
    echo "SRC directory does not exist."
    exit 3
fi
if [[ ! -d "$DST" ]]; then
    echo "DST directory does not exist."
    exit 4
fi
if [[ $(ls -A "$DST") ]]; then
    echo "DST directory is not empty."
    exit 5
fi

find "$SRC" -type f -name "*$SUB*" | while read src_file; do
а
    file_name=$(basename "$src_file")
    src_dir=$(dirname "$src_file")
    mkdir -p "$DST/$src_dir"
    mv "$src_file" "$DST/$src_dir/$file_name"
done
