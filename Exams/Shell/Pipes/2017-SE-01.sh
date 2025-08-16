#Намерете имената на топ 5 файловете в текущата директория с най-много hardlinks.

find . -type f -printf '%n %p\n' | sort -nr | head -n 5
