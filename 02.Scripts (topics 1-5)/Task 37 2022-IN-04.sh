#!/bin/bash

if [[ $# -ne 1 ]]; then
  exit 1;
fi

fuga="${1}
if [[ ! -d "$fuga" ]];then
  exit 1
fi

# 2) Файлове/директории в fuga

pwdFile="$fuga/foo.pwd"
validateFile="$fuga/validate.sh"
cfgDir="$fuga/cfg"
confFile="$fuga/foo.conf"

[[ -f "$pwdFile" ]] || exit 2
[[ -x "$validateFile" ]] || exit 2
[[ -d "$cfgDir" ]] || exit 2

# 3) Прегенерираме foo.conf (изчистваме го)
rm "$confFile"
touch "$confFile"

# 4) Намираме всички cfg файлове с име: малки букви + .cfg
# (ползваме basename проверка, защото find -name е glob, не regex)

while IFS= read -r -d '' file;do
  base=$(basename file)
  [[ "$base" =~ ^[a-z]+\.cfg$ ]] || continue
   # 5) Валидираме
   out=$("$validateFile" "$file")
   rc=$?

   if [[ $rc -eq 1 ]];then
      # невалиден: печатаме всички редове от валидатора на stderr с префикс "file:"
    # out съдържа редове "Line x: ..."
    while IFS= read -r line;do
      echo "$file:$line" >&2
    done< < "$out"
    elif [[ $rc -eq 0 ]];then
      # валиден: добавяме към foo.conf 
      cat "$file" >> "$confFile"
      user=${base%.cfg}
      if  ! -grep -q "^${user}:" "$pwdFile";then
        plain=$(pwgen 16 1)
        hash=$(mkpasswd "$plain")
        echo "${user}:${hash}" >> "$pwdFile"
        echo "${user}:${plain}"
      fi
      else
    # rc == 2 или друго: грешка във validate.sh -> не го считаме за валиден
    echo "$file:validate error" >&2
  fi
        
  done < <(find "$cfgDir" -type f -name '*.cfg' -print0)
