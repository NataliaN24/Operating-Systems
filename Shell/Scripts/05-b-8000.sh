Напишете shell script, който получава като единствен аргумент
име на потребител и за всеки негов процес изписва съобщение за 
съотношението на RSS към VSZ. Съобщенията да са сортирани, като процесите
с най-много заета виртуална памет са най-отгоре.

Hint:
Понеже в Bash няма аритметика с плаваща запетая, за смятането на съотношението
използвайте командата bc. За да сметнем нампример 24/7, можем да: 
echo "scale=2; 24/7" | bc
Резултатът е 3.42 и има 2 знака след десетичната точка, защото scale=2.
Алтернативно, при липса на bc ползвайте awk.

  1 #!/bin/bash
  2
  3 if [ $# -ne 1 ]; then
  4     exit 1
  5 fi
  6
  7 user="$1"
  8
  9 if ! id "$user" &>/dev/null; then
 10     echo "user does not exist"
 11     exit 1
 12 fi
 13
 14
 15
 16 ps -u "$user" -o pid=,rss=,vsz= | sort -k3 -nr |
 17         while read pid rss vsz; do
 18             if [ "$vsz" -gt 0 ]; then
 19                 ratio=$(echo "scale=2; $rss / $vsz" | bc)
 20             else
 21             ratio="N/A"
 22             fi
 23             echo "PID: $pid RSS:$rss VSZ : $vsz RATIO: $ratio"
 24         done
