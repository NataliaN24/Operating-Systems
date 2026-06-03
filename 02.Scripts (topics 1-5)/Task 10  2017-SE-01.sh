Напишете скрипт, който получава задължителен първи позиционен параметър – 
директория и незадължителен втори – число. Скриптът трябва да проверява
подадената директория и нейните под-директории и да извежда имената на:
а) при подаден на скрипта втори параметър – всички файлове с брой
hardlink-ове поне равен на
параметъра;
б) при липса на втори параметър – всички symlink-ове с несъществуващ 
destination (счупени symlinkове).
Забележка:За удобство приемаме, че ако има подаден втори параметър, то той е число.

#!/bin/bash

if [[ $# -lt 1 || $# -gt 2 ]]; then
    exit 1
fi

if [[ ! -d "$1" ]]; then
    exit 1
fi

if [[ $# -eq 2 ]]; then
    find "$1" -type f -printf '%n %p\n' | while read -r hardlink path; do
        if [[ "$hardlink" -ge "$2" ]]; then
            echo "$path"
        fi
    done
else
    find "$1" -type l -printf '%p\n' | while read -r path; do
        if [[ ! -e "$path" ]]; then
            echo "$path"
        fi
    done
fi
########################################################


#!/bin/bash

if [[ $# -lt 1 || $# -gt 2 ]]; then
    echo "Usage: $0 DIR [N]" >&2
    exit 1
fi

dir="$1"

if [[ ! -d "$dir" ]]; then
    echo "First argument must be directory" >&2
    exit 2
fi

tmp=$(mktemp)

if [[ $# -eq 2 ]]; then
    n="$2"

    find "$dir" -type f -printf '%n %p\n' > "$tmp"

    while read -r links file; do
        if [[ "$links" -ge "$n" ]]; then
            echo "$file"
        fi
    done < "$tmp"

else
    find "$dir" -type l -printf '%p\n' > "$tmp"

    while read -r link; do
        if [[ ! -e "$link" ]]; then
            echo "$link"
        fi
    done < "$tmp"
fi

rm -f "$tmp"

