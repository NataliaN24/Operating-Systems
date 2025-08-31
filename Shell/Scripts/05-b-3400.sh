#Да се напише shell скрипт, който чете от стандартния вход име на файл и символен низ, 
проверява дали низа се съдържа
#във файла и извежда на стандартния изход кода на завършване на командата с която сте
проверили наличието на низа.
#
#NB! Символният низ може да съдържа интервал (' ') в себе си.

  1 #!/bin/bash
  2
  3 echo "enter filename"
  4 read filename
  5 echo "enter string"
  6 read string
  7
  8 if [[ ! -f "$filename" ]];then
  9 echo "file does not exists"
 10 exit 1
 11 fi
 12
 13 grep -q "$string" "$filename"
 14
 15 exitCode=$?
 16 echo  "the code is:" $exitCode

 #or

   1 #!/bin/bash
  2
  3 read -p "enter the file name: " filename
  4 read -p "enter the string : "stringname
  5
  6
  7 if [ ! -f "$filename" ];then
  8     echo "file doesn't exist"
  9     exit 1
 10 fi
 11
 12     grep -q "${stringname}" "${filename}"
 13 if [ $? -eq 0 ]; then
 14     echo "the string was found within file"
 15 else
 16     echo "the string was not found"
 17 fi
 18
 19 echo "Exit code : $?"

