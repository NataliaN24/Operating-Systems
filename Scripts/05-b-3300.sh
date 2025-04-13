#Да се напише shell скрипт, който чете от стандартния вход имената на 3 файла,
#обединява редовете на първите два (man paste), подрежда ги по азбучен ред и
#резултата записва в третия файл.

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
