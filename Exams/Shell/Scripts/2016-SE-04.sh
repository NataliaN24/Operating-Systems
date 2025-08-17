#Файловете във вашата home директория съдържат информация за музикални албуми и имат специфична структура.
#Началото на всеки ред е годината на издаване на албума, а
#непосредствено, след началото на всеки ред следва името на изпълителя на песента. Имената на
#файловете се състоят от една дума, която съвпада с името на изпълнителя.
#Примерно съдържание на файл с име "Bonnie":
#5
#2005г. Bonnie - "God Was in the Water" (Randall Bramblett, Davis Causey) – 5:17
#2005г. Bonnie - "Love on One Condition" (Jon Cleary) – 3:43
#2005г. Bonnie - "So Close" (Tony Arata, George Marinelli, Pete Wasner) – 3:22
#2005г. Bonnie - "Trinkets" (Emory Joseph) – 5:02
#2005г. Bonnie - "Crooked Crown" (David Batteau, Maia Sharp) – 3:49
#2005г. Bonnie - "Unnecessarily Mercenary" (Jon Cleary) – 3:51
#2005г. Bonnie - "I Will Not Be Broken" - "Deep Water" (John Capek, Marc Jordan) – 3:58

#Да се напише shell скрипт приемащ два параметъра, които са имена на файлове от вашата home директория.
#Скриптът сравнява, кой от двата файла има повече на брой редове, съдържащи неговото
#име (на файла). За файлът победител изпълнете следните действия:
#• извлечете съдържанието му, без годината на издаване на албума и без името на изпълнителя
#• сортирайте лексикографски извлеченото съдържание и го запишете във файл с име ’изпълнител.songs’
#Примерен изходен файл (с име Bonnie.songs):
#"Crooked Crown" (David Batteau, Maia Sharp) – 3:49
#"God Was in the Water" (Randall Bramblett, Davis Causey) – 5:17
#"I Will Not Be Broken" - "Deep Water" (John Capek, Marc Jordan) – 3:58
#"Love on One Condition" (Jon Cleary) – 3:43
#"So Close" (Tony Arata, George Marinelli, Pete Wasner) – 3:22
#"Trinkets" (Emory Joseph) – 5:02
#"Unnecessarily Mercenary" (Jon Cleary) – 3:51

#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 <file1> <file2>"
    exit 1
fi

file1=$1
file2=$2

if [[ ! -f "$file1" || ! -f "$file2" ]]; then
    echo "One or both files do not exist."
    exit 2
fi
name1=$(basename "$file1")
name2=$(basename "$file2")

count1=$(grep -c  "$name1" "$file1")
count2=$(grep -c "$name2" "$file2")

if (( count1 > count2 )); then
  winner="$file1"
  winner_name="$name1"
else
  winner="$file2"
  winner_name="$name2"
fi

sed "s/^.*$winner_name - //" "$winner" | sort > "${winner_name}.songs"
echo "Output written to ${winner_name}.songs"
