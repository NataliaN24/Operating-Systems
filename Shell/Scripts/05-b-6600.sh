Да се напише shell скрипт, който получава единствен аргумент
директория и изтрива всички повтарящи се (по съдържание) файлове
в дадената директория. Когато има няколко еднакви файла, да се остави само този
, чието име е лексикографски преди имената на останалите дублирани файлове.

Примери:
$ ls .
f1 f2 f3 asdf asdf2
# asdf и asdf2 са еднакви по съдържание, но f1, f2, f3 са уникални

$ ./rmdup .
$ ls .
f1 f2 f3 asdf
# asdf2 е изтрит

 #!/bin/bash
  2
  3 if [ $# -ne 1 ]; then
  4     exit 1
  5 fi
  6
  7 dir=$1
  8
  9 if [ ! -d "$dir" ]; then
 10     exit 1
 11 fi
 12
 13 #Списък с всички файлове
 14
 15 files=$(find "$dir" -maxdepth 1 -type f | sort)
 16
 17 for f1 in $files; do
 18     for f2 in $files; do
 19        if [ "$f1" = "$f2" ]; then  #if they are the same file
 20         continue
 21        fi
 22         if cmp -s "$f1" "$f2"; then
 23         if [[ "$f1" < "$f2" ]]; then
 24             rm "$f2"
 25         else
 26              rm "$f1"
 27              break
 28         fi
 29     fi
 30     done
 31 done
