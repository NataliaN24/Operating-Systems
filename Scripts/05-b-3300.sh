#Да се напише shell скрипт, който чете от стандартния вход имената на 3 файла,
#обединява редовете на първите два (man paste), подрежда ги по азбучен ред и
#резултата записва в третия файл.

#better solution
 1 #!/bin/bash
  2
  3 echo "Enter the name of the three files:"
  4     read file1 file2 file3
  5
  6 if [ ! -f "$file1" ] && [ -f "$file2" ]; then
  7     echo "File 1 doesn't exist"
  8     cat "$file2" | sort > "$file3"
  9     exit 0
 10 fi
 11
 12 if [ ! -f "$file2" ] && [ -f "$file1" ]; then
 13     echo "File 2 doesn't exist"
 14     cat "$file1" | sort > "$file3"
 15 fi
 16
 17 if [ ! -f "$file1" ] && [ ! -f "$file2" ]; then
 18     echo "Both files don't exist"
 19     exit 1;
 20 fi
 21
 22   paste "$file1" "$file2" | sort  > "$file3"
 23     echo "Files are merged"


#or

 1 #!/bin/bash
  2  echo "enter file 1"
  3  read file1
  4  echo "enter file2"
  5  read file2
  6  echo "enter file3"
  7  read file3
  8
  9  if [[ ! -f "$file1"]]; then
 10  echo "file1does not exist"
 11  exit 1
 12  fi
 13
 14  if [[ ! -f "$file2"]]; then
 15  echo "file 2 does not exist"
 16  exit 1
 17  fi
 18
 19  if [[ ! -f "$ile3" ]]; then
 20  echo "file 3 does not exist"
 21  exit 1
 22  fi
 23
 24 paste "$file1" "$file2" | sort > "$file3"
 25
 26 echo "the results are saved in $file3"
 27
