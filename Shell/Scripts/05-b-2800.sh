#Да се напише shell скрипт, който приема точно един параметър и проверява дали подаденият му параметър
#се състои само от букви и цифри.

  1 #!/bin/bash
  2
  3 if [[ $# -ne 1 ]]; then
  4 echo "It must contain only one parameter"
  5 exit 1
  6 fi
  7
  8 param="$1"
  9 if [[ "$param" =~ ^[a-zA-Z0-9]+$ ]]; then
 10 echo "valid input"
 11 else
 12 echo "invalid input"
 13 fi

#Comments
=~ means is equal to the string on the right hand side
