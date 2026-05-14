#!/bin/bash

if [[ $# -ne 1]]; then
  exit 1
fi

if [[ ! -f bakefile ]];then
  exit 2
fi
<файл>:<зависимост 1> <зависимост 2> ... <зависимост N>:<команда>
chapter1.pdf:chapter1.md:pandoc -o chapter1.pdf chapter1.md
chapter2.pdf:chapter2.md drawing42.svg:pandoc -o chapter2.pdf chapter2.md
book.pdf:chapter1.pdf chapter2.pdf:pdfunite chapter1.pdf chapter2.pdf

file="$1"
rule=$(grep -m 1 "^$file:"  bakefile) #stop at first encounter

if [[ -z "$rule" ]]; then
    if [[ -f "$file" ]]; then
        exit 0
    else
        exit 3
    fi
fi

deps=$(echo "$rule" | cut -d ':' -f2)
cmd=$(echo "$rule" | cut -d ':' -f3)
for dep in $deps;do
  "$0" "$dep"
  if [[ $? -ne 0 ]]; then
        exit 4
    fi
done
if [[ ! -f "$file"]];then
  eval "$cmd"
  exit $?
fi

needBuild=0
for dep in $deps;do
  if [[ "$dep" -nt "$file" ]];then
    needBuild=1
    break
  fi
done
if [[ $need_build -eq 1 ]]; then
    eval "$cmd"
    exit $?
fi
exit 0
