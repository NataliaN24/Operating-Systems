Под конфигурационен файл ще разбираме файл, в който има редове от вида key=value,
където key и value могат да се състоят от букви, цифри и знак “долна черта” (“_”). Освен това, във
файла може да има празни редове; може да има произволен whitespace в началото и в края на редовете,
както и около символа “=”. Също така са допустими и коментари в даден ред: всичко след символ “#”
се приема за коментар.
Под <date> ще разбираме текущото време, върнато от командата date без параметри; под <user> ще
разбираме името на текущият потребител.
Напишете shell скрипт set_value.sh, който приема 3 позиционни аргумента – име на конфигурационен файл, ключ (foo) и стойност (bar). Ако ключът:
• присъства във файла с друга стойност, скриптът трябва да:
– да закоментира този ред като сложи # в началото на реда и добави в края на реда # edited
at <date> by <user>
– да добави нов ред foo = bar # added at <date> by <user> точно след стария ред
• не присъства във файла, скриптът трябва да добави ред от вида foo = bar # added at <date>
by <user> на края на файла
Примерен foo.conf:
# route description
from = Sofia
to = Varna # my favourite city!
type = t2_1
Примерно извикване:
./set_value.sh foo.conf to Plovdiv
17
Съдържание на foo.conf след извикването:
# route description
from = Sofia
# to = Varna # my favourite city! # edited at Tue Aug 25 15:48:29 EEST 2020 by human
to = Plovdiv # added at Tue Aug 25 15:48:29 EEST 2020 by human
type = t2_1

#!/bin/bash

if [[ $# -ne 3 ]]; then
    echo "Usage: $0 <config_file> <key> <value>"
    exit 1
fi

CONF="$1"
KEY="$2"
VALUE="$3"

DATE=$(date)
USER=$(whoami)

FOUND=0
NEW_FILE="${CONF}.new"

> "$NEW_FILE"

while IFS= read -r line || [[ -n "$line" ]]; do
    clean_line=$(echo "$line" | awk '{$1=$1; print}')  # премахваме водещи и крайни интервали

    if [[ -z "$clean_line" ]]; then
        echo "$line" >> "$NEW_FILE"
        continue
    fi

    if [[ "$clean_line" == "$KEY"* ]]; then
        echo "#$line # edited at $DATE by $USER" >> "$NEW_FILE"
        echo "$KEY = $VALUE # added at $DATE by $USER" >> "$NEW_FILE"
        FOUND=1
    else
        echo "$line" >> "$NEW_FILE"
    fi
done < "$CONF"

if [[ $FOUND -eq 0 ]]; then
    echo "$KEY = $VALUE # added at $DATE by $USER" >> "$NEW_FILE"
fi

mv "$NEW_FILE" "$CONF"
