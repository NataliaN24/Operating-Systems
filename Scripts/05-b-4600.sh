Да се напише shell скрипт, който валидира дали дадено цяло число попада в целочислен интервал.
Скриптът приема 3 аргумента: числото, което трябва да се провери; лява граница на интервала; дясна граница на интервала.
Скриптът да връща exit status:
- 3, когато поне един от трите аргумента не е цяло число
- 2, когато границите на интервала са обърнати
- 1, когато числото не попада в интервала
- 0, когато числото попада в интервала

Примери:
$ ./validint.sh -42 0 102; echo $?
1

$ ./validint.sh 88 94 280; echo $?
1

$ ./validint.sh 32 42 0; echo $?
2

$ ./validint.sh asdf - 280; echo $?
3

 #!/bin/bash
  2
  3 if [ $# -ne 3 ]; then
  4     echo "not three arguments"
  5     exit 1
  6 fi
  7 x=$1
  8 min=$2
  9 max=$3
 10
 11 isInteger(){
 12     [[ "$1" =~ ^-?[0-9]+$ ]]
 13 }
 14
 15 if ! isInteger "$x"  || ! isInteger  "$min" || ! isInteger "$max"; then
 16     echo "One of the arguments not an integer" #maybe the if conditions can be split to three if st
 17     exit 3
 18 fi
 19
 20 if [ "$min" -gt "$max" ]; then
 21     echo "Left bound is greater than right bound"
 22     exit 2
 23 fi
 24
 25 if [ "$x" -ge "$min" ] && [ "$x" -le "$max" ]; then
 26     echo "Within interval"
 27     exit 0
 28
 29 else
 30     echo "Outside interval"
 31     exit 1
 32 fi
