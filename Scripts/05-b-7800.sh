Да се напише shell скрипт, който намира броя на изпълнимите файлове в PATH.
Hint: Предполага се, че няма спейсове в имената на директориите
Hint2: Ако все пак искаме да се справим с този случай, да се разгледа IFS 
променливата и констуркцията while read -d


  1 #!/bin/bash
  2
  3 if [ $# -ne 1 ]; then
  4     echo "Expected 1 argument"
  5     exit 1
  6 fi
  7
  8 if [ ! -d "$1" ]; then
  9     echo " not a directory"
 10     exit 1
 11 fi
 12
 13 count=0
 14
 15 for file in "$1"/*; do
 16     if [ -f "$file" ]; then
 17         if [ -x "$file" ]; then
 18          ((count++))
 19         fi
 20     fi
 21 done
 22  echo "$count"
