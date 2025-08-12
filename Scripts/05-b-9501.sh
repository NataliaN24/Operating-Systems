Този път програмата ви ще приема само един параметър,
който е измежду ("-r", "-b", "-g", "-x").
Напишете shell script, който приема редовете от 
stdin и ги изпринтва всеки ред с редуващ се цвят.
Цветовете вървят RED-GREEN-BLUE и цветът на първия ред се определя от аргумента.
 Ако е подаден аргумент "-x", то не трябва да променяте 
 цветовете в терминала (т.е., все едно сте извикали командата cat).

Hint: Не забравяйте да връщате цветовете в терминала.

  1 #!/bin/bash
  2
  3 if [ $# -ne 1 ]; then
  4     exit 1
  5 fi
  6
  7 #color codes
  8
  9 RED='\033[0;31m'
 10 GREEN='\033[0;32m'
 11 BLUE='\033[0;34m'
 12 RESET='\033[0m'
 13
 14 arg=$1
 15
 16 if [ "$arg" == "-x" ]; then
 17     cat
 18     exit 0
 19 fi
 20
 21 #decide starting color index
 22
 23 case "$arg" in
 24     -r) start=0 ;;
 25     -g) start=1 ;;
 26     -b) start=2 ;;
 27     *)
 28         echo "unknown"
 29         exit 1
 30     ;;
 31 esac
 32
 33 #color array
 34
 35 colors=("$RED" "$GREEN" "$BLUE")
 36
 37 index=$start
 38 while IFS= read -r line; do
 39     echo -e "${colors[$index]}$line${RESET}"
 40     index=$(( ( index + 1) % 3 ))
 41 done
 42
