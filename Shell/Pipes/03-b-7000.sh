#да приемем, че файловете, които съдържат C код, завършват на `.c` или `.h`.
#Колко на брой са те в директорията `/usr/include`?
#Колко реда C код има в тези файлове?

find /usr/include -type f \( -name "*.c" -o -name "*.h" \) | wc -l
find /usr/include -type f \( -name "*.c" -o -name "*.h" \) | xargs cat | wc -l

#second option better

find /usr/include -name '*.[ch]' |  wc -l
cat $(find /usr/include -name '*.[ch]') |  wc -l
