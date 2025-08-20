Напишете shell скрипт, който приема 3 позиционни аргумента – две имена на файлове и
име на директория. Примерно извикване:
$ ./foo.sh foo.pwd config.cfg cfgdir/
В директорията cfgdir/ и нейните под-директории може да има файлове с имена завършващи на
.cfg. За да са валидни, тези файлове трябва да съдържат редове само в следните формати (редовете
започващи с # са коментари):
# internal laboratory
{ no-production };
16
{ volatile };
# meow configs
{ run-all; };
Във файла foo.pwd има описани потребителски имена (username) и MD5 хеш суми на паролите им,
с по един запис на ред, в следният формат:
username:password_hash
Също така, разполагате с команда pwgen, която генерира и извежда на STDOUT случайни пароли, и
знаете, че поддържа следните два аргумента:
$ pwgen [ password_length ] [ number_of_passwords ]
Вашият скрипт трябва да валидира cfg файловете в директорията, и за тези, които не са валидни, да
извежда на STDOUT името на файла и номерирани редовете, които имат проблем, в следния формат:
Error in filename.cfg:
Line 1:XXXX
Line 37:YYYY
където XXXX и YYYY е съдържанието на съответния ред.
За валидните файлове, скриптът трябва да:
• генерира config.cfg като обединение на съдържанието им;
• името на файла, без частта .cfg дефинира потребителско име. Ако във файла с паролите не
съществува запис за този потребител, то такъв да се добави и на стандартния изход да се изведе
потребителското име и паролата (поне 16 символа) разделени с един интервал.


#!/bin/bash

if [[ $# -ne 3 ]]; then
    echo "Usage: $0 <password_file> <output_cfg> <cfg_dir>"
    exit 1
fi

PWD_FILE="$1"
OUT_CFG="$2"
CFG_DIR="$3"

if [[ ! -d "$CFG_DIR" ]]; then
    echo "Directory $CFG_DIR does not exist"
    exit 1
fi

#  масив с потребителите от foo.pwd
declare -A users
if [[ -f "$PWD_FILE" ]]; then
    while IFS=: read -r user hash; do
        users["$user"]="$hash"
    done < "$PWD_FILE"
fi

> "$OUT_CFG"

find "$CFG_DIR" -type f -name "*.cfg" | while IFS= read -r cfg; do
    filename=$(basename "$cfg")
    line_num=0
    valid=1
    errors=()

    while IFS= read -r line || [[ -n "$line" ]]; do
        line_num=$((line_num+1))
        clean_line=$(echo "$line" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')

        if [[ -z "$clean_line" || "$clean_line" =~ ^# ]]; then
            continue
        elif [[ "$clean_line" =~ ^\{.*\};$ ]]; then
            continue
        elif [[ "$clean_line" =~ ^[0-9]+$ ]]; then
            continue
        else
            errors+=("Line $line_num:$line")
            valid=0
        fi
    done < "$cfg"

    if [[ $valid -eq 0 ]]; then
        echo "Error in $filename:"
        for e in "${errors[@]}"; do
            echo "$e"
        done
    else
        # Файлът е валиден → добавяме съдържанието му към OUT_CFG
        cat "$cfg" >> "$OUT_CFG"

        # Потребителско име = filename без .cfg
        user="${filename%.cfg}"

        # Проверка дали потребителят вече съществува
        if [[ -z "${users[$user]}" ]]; then

            password=$(pwgen 16 1)
            echo "$user $password"
            users["$user"]="new"  
        fi
    fi
done
