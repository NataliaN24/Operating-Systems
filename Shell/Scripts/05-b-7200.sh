Да се напише shell скрипт, който приема произволен брой аргументи - 
имена на файлове или директории. Скриптът да извежда за всеки аргумент
подходящо съобщение:
	- дали е файл, който може да прочетем
	- ако е директория - имената на файловете в нея, които имат размер, по-малък
 :от броя на файловете в директорията.

  1 #!/bin/bash
  2
  3 if [ $# -lt 1 ]; then
  4     echo " error 1"
  5     exit 1
  6 fi
  7
  8
  9 for arg in "$@"; do
 10     if [ -f "$arg" ]; then
 11         if [ -r "$arg" ]; then
 12             echo " $arg is a readable file "
 13         else
 14             echo " $arg is a file but not readable"
 15         fi
 16
 17     elif [ -d "$arg" ]; then
 18     count=$(find "$arg" -type f | wc -l)
 19     echo "$arg is a directory with $count files"
 20
 21     for file in "$arg"/*; do
 22         if [ -f "$file" ]; then
 23             size=$(stat -c%s "$file")
 24             if [ "$size" -lt "$count" ]; then
 25                 echo "   $file (size: $size bytes) "
 26             fi
 27         fi
 28     done
 29     fi
 30 done
