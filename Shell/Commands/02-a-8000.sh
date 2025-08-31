#Копирайте всички файлове от /etc, които могат да се четат от всички, в
#директория myetc в home директорията ви. Направете такава, ако нямате

#first solution
find /etc -perm -444 -exec cp {} ./commands_tasks/myetc ';'

#second solution
find /etc/ -perm /u+r,g+r,o+r -exec cp -r {} ./commands_tasks/myetc/ \;
