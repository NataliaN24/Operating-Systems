#Вземете факултетните номера на студентите (описани във файла
#<РЕПО>/exercises/data/mypasswd.txt) от СИ и ги запишете във файл si.txt сортирани.
#
#Студент е част от СИ, ако home директорията на този потребител (според
#<РЕПО>/exercises/data/mypasswd.txt) се намира в /home/SI директорията.

cat /srv/fmi-os/exercises/data/mypasswd.txt | grep 'home/SI' | cut -f 1 -d ':' | sort -n > pipes_tasks/si.txt

#or
cat /srv/fmi-os/exercises/data/mypasswd.txt | grep '/home/SI' | cut -d ':' -f1 | cut -c 2- | grep -v '[a-zA-Z]' | sort -nr
