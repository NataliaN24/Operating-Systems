#Намерете само Group ID-то си от файлa /etc/passwd.

 grep "^$(whoami)" /etc/passwd | cut -f4 -d ':'

