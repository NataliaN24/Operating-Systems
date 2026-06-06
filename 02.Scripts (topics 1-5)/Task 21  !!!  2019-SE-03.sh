#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

if [[ ! -d "$1" ]]; then
    exit 2
fi

STATE=".statesha256"
NEWSTATE=".stateshaNEW"

mkdir -p /extracted
touch "$STATE"
> "$NEWSTATE"

find "$1" -type f | grep -E '.*/[^/_]+_report-[0-9]+\.tgz$' | while read archive; do

    filename=$(basename "$archive")

    name=$(echo "$filename" | cut -d '_' -f1)

    timestamp=$(echo "$filename" | cut -d '-' -f2 | cut -d '.' -f1)

    hash=$(sha256sum "$archive" | cut -d ' ' -f1)

    oldhash=$(grep -F "  $archive" "$STATE" | cut -d ' ' -f1)

    echo "$hash  $archive" >> "$NEWSTATE"

    if [[ "$oldhash" != "$hash" ]]; then
        if tar -tzf "$archive" | grep -qx 'meow.txt'; then
            tar -xOzf "$archive" meow.txt > "/extracted/${name}_${timestamp}.txt"
        fi
    fi

done

mv "$NEWSTATE" "$STATE"
______________________________________________________________________________________________



#!/bin/bash


#IMPORTANT : НЕ трябва да обработва архиви, които вече са били обработени и не
#са променени.
#Това означава, че трябва да пазим информация от предишното пускане.
#Ако съдържанието се промени → хешът се променя.

if [[ $# -ne 1 ]]; then
  echo "one argument needed" >&2
  exit 1;
fi

dir="${1}"

if [[ ! -d "$dir" ]]; then
  echo "not a dir" >&2
  exit 2
fi

mkdir -p /extracted

STATE="/tmp/sha_state.txt" # Трябва да пазиш хешовете от предишното пускане в файл
touch "$STATE"
TMP_STATE=$(mktemp) #за теукщото състояние

# 4) Намираме всички .tgz архиви рекурсивно (без да се чупим от интервали)

find "$dir" -type f -name '*.tgz' -print0
while IFS= read -rd ' ' file;do
  base=$(basename file)
    # 5) Филтър: приемаме само файлове с име NAME_report-TIMESTAMP.tgz
  # NAME: без '_', TIMESTAMP: само цифри
  
if [[ echo "$base" | grep -Eq '^[^_]+_report-[0-9]+\.tgz$']];then
 # не е от желания тип -> пропускаме
   continue;
fi
name=$(echo "$base" | cut -d '_' -f1
tmp=$(echo $base" | cut -d '-' -f2)
timestamp=${tmp%.tgz}

#tекущ sha256 за архива

curLine=$(sha256sum "$file")
currhash=${curLine%%*}

  # 9) Записваме текущото състояние в TMP_STATE (за да стане новият STATE накрая)
  echo "$currHash $file" >> "$TMP_STATE"

   # 10) Проверка "нов или променен по съдържание спрямо предното пускане"
  # Ако редът (hash + два интервала + path) НЕ съществува в STATE -> обработваме

if   [[ grep -q "^$currHash "$file$" "$STATE"; then

# 11) Проверяваме дали вътре в архива има meow.txt

  if [[tar -tzf "$file" 2>/dev/null | grep -qx "meow.txt"]]; then
    out="/extracted/${name}_${timestamp}.txt"
      # 12) Извличаме meow.txt и го записваме в желания файл
      # -O: праща съдържанието на meow.txt на stdout, а ние го пренасочваме към файла
      
    tar -xzf "$file" -O "meow.txt" >"$out"
  fi
fi
fi
done
# 13) Накрая сменяме state-а: новият списък от хешове става "предишно пускане"
mv "$TMP_STATE" "$STATE"
