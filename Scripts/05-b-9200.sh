Да се напише shell скрипт, който получава произволен брой аргументи файлове, които изтрива.
Ако бъде подадена празна директория, тя бива изтрита. Ако подадения файл е директория с поне 1 файл, тя не се изтрива.
За всеки изтрит файл (директория) скриптът добавя ред във log файл с подходящо съобщение.

а) Името на log файла да се чете от shell environment променлива, която сте конфигурирали във вашия .bashrc.
б) Добавете параметър -r на скрипта, който позволява да се изтриват непразни директории рекурсивно.
в) Добавете timestamp на log съобщенията във формата: 2018-05-01 22:51:36

Примери:
$ export RMLOG_FILE=~/logs/remove.log
$ ./rmlog -r f1 f2 f3 mydir/ emptydir/
$ cat $RMLOG_FILE
[2018-04-01 13:12:00] Removed file f1
[2018-04-01 13:12:00] Removed file f2
[2018-04-01 13:12:00] Removed file f3
[2018-04-01 13:12:00] Removed directory recursively mydir/
[2018-04-01 13:12:00] Removed directory emptydir/

 #!/bin/bash
  2
  3 if [ $# -lt 1 ]; then
  4     exit 1;
  5 fi
  6
  7 if [ -z "$RMLOG_FILE" ]; then
  8     echo " rmlog_file  variable is not set"
  9     exit 1
 10 fi
 11
 12 #flag for recursive search
 13 recursive=false
 14
 15 #check if -r is a first argument
 16 if [ "$1" == "-r" ]; then
 17     recursive=true
 18     shift     #means that $2 will be the new $1
 19 fi
 20
 21 log_action() {
 22     local message="$1"    #local means that this variable will exist only inside the scope of this function
 23     local timestamp=$(date '+%Y-%m-%d %H:%M:%S')
 24     echo "[$timestamp] $message" >> "$RMLOG_FILE" # >> means append to the end of file
 25 }
 26
 27 for path in "$@"; do
 28     if [ -f "$path" ]; then
 29         rm "$path"
 30         log_action "removed file $path"
 31     elif [ -d "$path" ]; then
 32         if [ "$recursive" = true ]; then
 33             rm -r "$path"
 34             log_action "removed directory recursively $path "
 35     else
 36
 37     #check if directory is empty
 38
 39         if [ -z "$(ls -A "$path")" ]; then  #ls -A shows all the files even the hidden ones
 40             rmdir "$path"
 41             log_action "removed directory $path"
 42         else
 43             echo "directory is not empty"
 44         fi
 45     fi
 46 else
 47 echo "no such file or directory"
 47 fi
 48 done


