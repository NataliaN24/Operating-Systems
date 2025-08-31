Да се напише shell скрипт, който приема произволен брой аргументи - имена на файлове. 
Скриптът да прочита от стандартния вход символен низ и за всеки от зададените файлове извежда по 
подходящ начин на стандартния изход броя на редовете, които съдържат низа.

NB! Низът може да съдържа интервал.


 1 #!/bin/bash
  2
  3 if [ $# -lt 1 ]; then
  4     exit 1
  5 fi
  6
  7 read string
  8
  9 # the symbol $@ means evry symbol given as argument
 10
 11 for file in "$@"; do
 12     if [ ! -f "$file" ]; then
 13         echo "not a file"
 14             continue       #because we'll check if other arguments are files
 15         fi
 16
 17     count=$(grep -F  "$string" "$file" | wc -l)  #grep -F because we want to interpret the string as a fixed one
 18     echo "$file: $count"
 19 done
