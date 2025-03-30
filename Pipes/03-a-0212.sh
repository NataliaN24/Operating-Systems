#Отпечатайте потребителските имена и техните home директории от /etc/passwd.

#Solution 1
cat /etc/passwd | cut -f 1,6 -d ':'| tr ':' ' '

#Solution 2
cat /etc/passwd | cut -f 1,6 -d ':'
