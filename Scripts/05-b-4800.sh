Да се напише shell скрипт, който приема файл и директория.
Скриптът проверява в подадената директория и нейните под-директории 
дали съществува копие на подадения файл и отпечатва имената на намерените 
копия, ако съществуват такива.
NB! Под 'копие' разбираме файл със същото съдържание.

 1 #!/bin/bash
  2
  3 if [ $# -ne 2 ];then
  4     echo "2 arguments !!! "
  5     exit 1
  6 fi
  7
  8 file=$1
  9 dir=$2
 10
 11 if [ ! -f "$file" ]; then
 12     echo "not a file"
 13     exit 1
 14 fi
 15
 16 if [ ! -d "$dir" ]; then
 17     echo "not directory"
 18     exit 1
 19 fi
 20
 21 for f in $(find "$dir" -type f); do
 22    #the case when it is exact file not the copied one
 23    if [ "$f" = "$file" ]; then
 24         continue
 25     fi
 26     if cmp -s "$file" "$f"; then
 27     echo "$f"
 28     fi
 29 done
