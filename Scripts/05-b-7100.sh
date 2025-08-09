Да се напише shell скрипт, който приема два параметъра -
име на директория и число. Скриптът да извежда на стандартния
изход имената на всички обикновени файлове във директорията,
които имат размер, по-голям от подаденото число.

  1 #!/bin/bash
  2
  3 if [ $# -ne 2 ]; then
  4         echo " error1"
  5         exit 1
  6 fi
  7
  8 dir=$1
  9 number=$2
 10
 11 if [ ! -d "$dir" ];then
 12         echo " error 2"
 13         exit 1
 14 fi
 15
 16 for files in "$dir"/*; do
 17     if [ ! -f "$files" ]; then
 18         continue
 19     fi
 20
 21     size=$(stat -c%s "$files")
 22     if [ "$size" -gt "$number" ]; then
 23         echo "$files $size"
 24     fi
 25
 26 done

 24 done
