 Напишете shell скрипт, който по подаден един позиционен параметър, ако този параметър
е директория, намира всички symlink-ове в нея и под-директориите ѝ с несъществуващ destination

#!/bin/bash

if [[$# -ne 1 ]];then
  echo "script must take only 1 arg"
  exit 1
fi

if[[ !-d "$1"]];then
  echo "arg must be a dir"
  exit 2
fi
echo"symbolic links:"
find "$1" -mindepth 1 -maxdepth 1 -type l 

echo "sub directories with non exisiting destination:"
find "$1" -mindepth 1 -maxdepth 1 -type l -exec  test -e {} \;-print
