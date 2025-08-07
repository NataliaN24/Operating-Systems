Да се напише shell скрипт, който форматира големи числа, за да са по-лесни за четене.
Като пръв аргумент на скрипта се подава цяло число.
Като втори незадължителен аргумент се подава разделител. По подразбиране цифрите се разделят с празен интервал.

Примери:
$ ./nicenumber.sh 1889734853
1 889 734 853

$ ./nicenumber.sh 7632223 ,
7,632,223

 #!/bin/bash
  2
  3 if [ $# -lt 1 ]; then
  4     echo "At least one argument"
  5     exit 1
  6 fi
  7
  8 number=$1
  9 sep=" "
 10
 11 if [ $# -ge 2 ];then
 12     sep="$2"
 13 fi
 14
 15 if ! [[ "$number" =~ ^-?[0-9]+$ ]]; then
 16     echo "Number is not an integer"
 17     exit 2
 18 fi
 19
 20 prefix=""
 21 if [[ "$number" =~ ^- ]]; then
 22     prefix="-"
 23     number="${number#-}"
 24 fi
 25
 26 formatted=$(echo "$number" | rev | sed -E "s/.{3}/&${sep}/g" | rev)
 27
 28 #if separator is added in the beggining in the case when the length can be divided by 3)
 29 if [[ "$formatted" == "${sep}"* ]]; then
 30     formatted="${formatted#$sep}"
 31 fi
 32
 33 echo "${prefix}${formatted}"
