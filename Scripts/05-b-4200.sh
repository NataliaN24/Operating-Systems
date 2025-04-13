#Имате компилируем (a.k.a няма синтактични грешки) source file на езика C. Напишете shell script, който да покaзва
#колко е дълбоко най-дълбокото nest-ване (влагане).
#Примерен .c файл:
#
##include <stdio.h>
#
#int main(int argc, char *argv[]) {
#
#  if (argc == 1) {
#		printf("There is only 1 argument");
#	} else {
#		printf("There are more than 1 arguments");
#	}
#
#	return 0;
#}
#Тук влагането е 2, понеже имаме main блок, а вътре в него if блок.
#
#Примерно извикване на скрипта:
#
#./count_nesting sum_c_code.c
#
#Изход:
#The deepest nesting is 2 levels deep.


 1 #!/bin/bash
  2
  3 #check if the file exists
  4 if [[ $# -ne q ]]; then
  5     echo "expected 1 argument"
  6     exit 1
  7 fi
  8
  9 currentDepth=0
 10 maxDepth=0
 11
 12
 13 #read the file line by line
 14
 15 while read ch; do
 16     if [[ ${ch}== "{" ]] ; then
 17         currentDepth=$((currentDepth + 1))
 18     else
 19          if [[ maxDepth -lt currentDepth ]]; then
 20              max=${currentDepth}
 21          fi
 22
 23         currentDepth=$(( currentDepth - 1 ))
 24     fi
 25
 26 done < < (cat "${1}" | grep -E -o '[{}]')
 27 echo "deepest nesting: $maxDepth


 ##explanation
 cat "${1}" reads the content of the file specified by the first argument (${1}).

grep -E -o '[{}]' filters the content, keeping only the { and } characters.

< <( ... ) redirects the filtered content as input for the while read ch; do ... done loop, which processes each brace one by one and tracks the nesting depth.

while read ch; do
    if [[ ${ch} == "{" ]]; then
        currentDepth=$((currentDepth + 1))  # Increase depth when '{' is found
    else
        if [[ maxDepth -lt currentDepth ]]; then
            maxDepth=${currentDepth}  # Update maxDepth if the current depth is greater
        fi
        currentDepth=$((currentDepth - 1))  # Decrease depth when '}' is found
    fi
done < <(cat "${1}" | grep -E -o '[{}]')

