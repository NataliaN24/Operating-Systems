#Да се напише shell скрипт, който приканва потребителя да въведе низ - потребителско име на потребител от
#системата - след което извежда на стандартния изход колко активни сесии има потребителят в момента.

  1 #!/bin/bash
  2
  3 echo "enter username"
  4 read username
  5
  6 if ! grep -q "${username}" /etc/passwd; then
  7 echo"no such username exists"
  8 exit 1
  9 fi
 10
 11 active_sessions=$(who | grep -w "$username" | wc -l)
 12
 13 echo "User $username has $active_sessions active sessions"

#Explanation.....................................................................................
who | grep -w "$username" | wc -l:

who: Извежда списък с всички активни сесии в системата.

grep -w "$username": Филтрира само редовете, които съдържат точно името на потребителя.

wc -l: Преброява колко реда съдържа изхода на командата, което означава колко активни сесии има този потребител.
