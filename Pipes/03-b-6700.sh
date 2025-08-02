#Отпечатайте правата (permissions) и имената на всички файлове, до които имате
#read достъп, намиращи се в директорията /tmp. (hint: 'man find', вижте -readable)

$find /tmp -type f -readable 2 > /dev/null | ls -l

#or

$ find /tmp -type f -readable 2>/dev/null | xargs ls -l
