#!/bin/bash

if [[ $# -ne 2 ]]; then
  echo "Usage: $0 <directory> <arch>" >&2
  exit 1
fi
dir="${1}"
str="${2}"

# 2) Проверки дали директорията съществува и е директория
if [[ ! -d "$dir" ]]; then
  echo "Error: '$dir' is not a directory" >&2
  exit 2
fi
# 3) Намираме само файловете в dir (без поддиректории),
#    филтрираме само vmlinuz-x.y.z-arch за даден arch,
#    сортираме по версия (x.y.z) с version-sort,
#    взимаме най-голямата (последния ред).
#sort -V го третирай като версия 4.12.4 > 4.11.12 (правилно) а не като текстово сравнение.

find "$dir" -type f -mindepth1 -maxdepth 1 -printf '%f\n'| grep -E "^vmlinuz-[0-9]+\.[0-9]+\.[0-9]+-${str}$"| sort -t '-' -k2,2V | tail -n 1
