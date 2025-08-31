#Да се напише shell скрипт, който приканва потребителя да въведе низ (име) и изпечатва "Hello, низ".

#First solution
  2 #!/bin/bash
  3
  4 echo -n "Enter your name:"
  5 read name
  6 echo "Hello $name"

#Second solution
read -p "Enter your name: " name
echo "Hello, ${name}"

#or
  1 #!/bin/bash
  2
  3 read name
  4 echo "Hello $name"
