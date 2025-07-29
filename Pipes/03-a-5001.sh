#Изведете колко потребители не изпозват /bin/bash за login shell според /etc/passwd
#(hint: 'man 5 passwd' за информация какъв е форматът на /etc/passwd)

#Solution 1
 cat /etc/passwd | grep -v "/bin/bash" | wc -l

#Solution 2
cat /etc/passwd | cut -d ':' -f 7 | grep -v '/bin/bash' | wc -l
