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
