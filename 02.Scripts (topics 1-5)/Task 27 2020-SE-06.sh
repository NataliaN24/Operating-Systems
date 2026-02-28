#!/bin/bash

# 1) Проверка: трябва да имаме точно 3 аргумента
# $1 = foo.pwd, $2 = config.cfg, $3 = cfgdir/
if [[ $# -ne 3 ]]; then
  exit 1
fi

file="$1"
key="$2"    #foo=bar
val="$3"

if [[ ! -f "$file" ]];then
  exit 2
fi

if [[ echo "$key" | grep -Eq '^[a-zA-Z0-9_]+$]];then
 exit 1;
fi

if [[ echo "$val" | grep -Eq '^[a-zA-Z0-9_]+$]];then
 exit 1;
fi

date=$(date)
user=$(whoami)
tmp=$(mktemp)
found=0
WHILE IFS= read -r line; do
  if [[ echo "$line " | grep -Eq '^[[:space:]]*#];then  #if comment add it as it is
    echo "$line"> "$tmp"
    continue;
  fi

no_comment=${line%%#*}

  # 6.3) Проверяваме дали "no_comment" изглежда като key=value (с whitespace около '=')
  # regex: ^\s*[A-Za-z0-9_]+\s*=\s*[A-Za-z0-9_]+\s*$ 
if [[ echo "$no_comment" | grep -Eq '^[[:space:]]*([A-Za-z0-9_])+[[:space:]]*=[[:space:]]*[A-Za-z0-9_]+[[:space:]]*$']];then
   current_key="$(echo "$no_comment" | sed -E 's/^[[:space:]]*([A-Za-z0-9_]+)[[:space:]]*=.*$/\1/')"
   current_val="$(echo "$no_comment" | sed -E 's/^[[:space:]]*[A-Za-z0-9_]+[[:space:]]*=[[:space:]]*([A-Za-z0-9_]+)[[:space:]]*$/\1/')"
if [[ "$current_key" == "$key" && $found -eq 0 ]]; then
  found =1;
  if [[ "$current_val" != "$val" ]];then
    echo "# $line #edited at $now by $user" >> "$tmp"
     # (b) Нов ред точно след него: "key = val # added at ... by ..."
        echo "$key = $val # added at $now by $user" >> "$tmp"
continue
fi
# Ако стойността е същата -> оставяме реда непроменен
fi
fi
echo "$line" >> "$tmp"

done < "$file"
# 7) Ако ключът не е намерен в активен ред -> добавяме на края
if [[ $found -eq 0 ]]; then
  echo "$key = $val # added at $now by $user" >> "$tmp"
fi

# 8) Заместваме файла атомарно
mv "$tmp" "$conf"

