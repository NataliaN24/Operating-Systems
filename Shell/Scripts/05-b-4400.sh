#Напишете shell script, който да приема параметър име на директория, от която взимаме файлове,
#и опционално експлицитно име на директория, в която ще копираме файлове. Скриптът да копира файловете със съдържание,
#променено преди по-малко от 45 мин, от първата директория във втората директория. Ако втората директория не е подадена 
#по име, нека да получи такова от днешната дата във формат,
#който ви е удобен. При желание новосъздадената директория да се архивира.

  1 #!/bin/bash
  2
  3 if [ "$#" -lt 1 ]; then
  4     echo "Expected at least 1 argument"
  5     exit 1
  6 fi
  7
  8 dirName="$1"
  9 copyDir="$2"
 10
 11 if [ ! -d "$dirName" ]; then
 12     echo "It is not a directory"
 13     exit 1
 14 fi
 15
 16  # if it doesn't exist
 17
 18 if [ -z "$copyDir" ]; then
 19     copyDir=$(date +%F)
 20     mkdir -p "$copyDir"
 21 else
 22 #if it is not a directory
 23     if [ ! -d "$copyDir" ]; then
 24         mkdir -p "$copyDir"
 25         fi
 26 fi
 27
 28
 29 find  "$dirName" -type f -mmin -45 -exec cp {} "$copyDir" \;
