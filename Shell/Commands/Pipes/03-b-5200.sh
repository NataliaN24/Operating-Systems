#Намерете броя на символите, различни от буквата 'а' във файла /etc/passwd

tr -d 'a' < /etc/passwd | wc -m
#or
cat /etc/passwd | sed 's/а//g' | wc -m
