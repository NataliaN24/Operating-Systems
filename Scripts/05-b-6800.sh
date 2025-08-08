Да се напише shell скрипт, който получава единствен аргумент директория и 
отпечатва списък с всички файлове и директории в нея (без скритите).
До името на всеки файл да седи размера му в байтове, а до името на всяка 
директория да седи броят на елементите в нея (общ брой на файловете и директориите, 
без скритите).

a) Добавете параметър -a, който указва на скрипта да проверява и скритите файлове
и директории.

Пример:
$ ./list.sh .
asdf.txt (250 bytes)
Documents (15 entries)
empty (0 entries)
junk (1 entry)
karh-pishtov.txt (8995979 bytes)
scripts (10 entries)

 1 #!/bin/bash
  2
  3 if [ $# -ne 1 ]; then
  4     exit 1
  5 fi
  6
  7 dir=$1
  8
  9 if [ ! -d "$dir" ]; then
 10     exit 1
 11  fi
 12 files=$(find "$dir" -type f)
 13 dirs=$(find "$dir" -type d)
 14
 15  for f in $files ; do
 16     size=$(stat -c%s "$f")
 17     echo "$f $size"
 18 done
 19
 20 for d in $dirs; do
 21    #skip the directory in which we are searching
 22    if [ "$d" = "$dir" ]; then
 23    continue
 24    fi
 25     count=$(find "$d"| wc -l)
 26     count=$((count-1))
 27     echo "$d $count"
 28 done
