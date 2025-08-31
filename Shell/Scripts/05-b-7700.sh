Да се напише shell скрипт, който приема два параметъра 
- име на директория и число. Скриптът да извежда сумата от
размерите на файловете в директорията, които имат
размер, по-голям от подаденото число.

 1 #!/bin/bash
  2
  3 if [ $# -ne 2 ]; then
  4     exit 1
  5 fi
  6
  7 dir=$1
  8 number=$2
  9 sum=0
 10
 11 if [ ! -d "$dir" ]; then
 12     exit 1
 13 fi
 14
 15 for file in "$dir"/*; do
 16     if [ -f "$file" ]; then
 17         size=$(stat -c%s "$file")
 18         if [ "$size" -gt "$number" ]; then
 19             sum=$((sum + size))
 20         fi
 21     fi
 22 done
 23
 24 echo " sum is "$sum" bytes"
 25
 26

