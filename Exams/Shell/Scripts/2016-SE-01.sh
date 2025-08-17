#Напишете shell скрипт, който по подаден един позиционен параметър, ако този
#параметър е директория, намира всички symlink-ове в нея и под-директориите `и с несъществуващ
#destination.

#!/bin/bash

if [[ $# -ne 1 ]]; then
  exit 1;
fi

$1=dir

if [[ ! -d "$dir" ]]; then
  echo "Not a directory"
  exit 2
fi

find "$dir" -type l ! -exec test -e {} \; -print

