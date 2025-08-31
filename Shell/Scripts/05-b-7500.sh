Напишете shell script guess, която си намисля число, което вие трябва да познате.
В зависимост от вашия отговор, програмата трябва да ви казва "надолу" или "нагоре", 
докато не познате числото. Когато го познаете, програмата да ви казва с колко 
опита сте успели.

./guess (програмата си намисля 5)

Guess? 22
...smaller!
Guess? 1
...bigger!
Guess? 4
...bigger!
Guess? 6
...smaller!
Guess? 5
RIGHT! Guessed 5 in 5 tries!

Hint: Един начин да направите рандъм число е с $(( (RANDOM % b) + a  )),
което ще генерира число в интервала [a, b]. Може да вземете a и b като параметри, 
но не забравяйте да направите проверката.

  1 #!/bin/bash
  2
  3 echo "enter lower bound"
  4 read lower
  5 echo "enter upper bound"
  6 read upper
  7
  8 if [ "$lower" -gt "$upper" ]; then
  9     echo "lower > upper"
 10     exit 1
 11 fi
 12 randomNumber=$(( RANDOM % (upper - lower +1) + lower ))
 13
 14 count=0
 15
 16 while true; do
 17 read -p "Guess " input
 18         ((count++))
 19
 20     if [ "$input" -lt "$randomNumber" ]; then
 21         echo " ... bigger"
 22
 23     elif [ "$input" -gt "$randomNumber" ]; then
 24         echo " ...smaller"
 25     else
 26         echo "RIGHT ! Guesses "$randomNumber" in "$count" tries !"
 27         break
 28     fi
 29 done
 30
 31 exit 0
