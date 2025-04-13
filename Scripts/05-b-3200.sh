#Да се напише shell скрипт, който приканва потребителя да въведе пълното име на директория и извежда на стандартния
#изход подходящо съобщение за броя на всички файлове и всички директории в нея.
 1 #!/bin/bash
  2
  3 echo "enter the name of directory"
  4 read dir
  5
  6 if [[ ! -d "$dir" ]];then
  7     echo "directory does not exist"
  8     exit 1
  9 fi
 10
 11 file_count=$(find "$dir" -type f | wc -l)
 12 dir_count=$(find "$dir" -type d | wc -l)
 13
 14 echo" Files number:$file_count Directories number:$dir_count"
 15


