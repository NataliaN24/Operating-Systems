# 1) Проверка: трябва да имаме точно 3 аргумента
# $1 = foo.pwd, $2 = config.cfg, $3 = cfgdir/
if [[ $# -ne 3 ]]; then
  exit 1
fi

pwdfile="$1"
outcfg="$2"
cfgdir="$3"

# 2) Проверки за входа
if [[ ! -f "$pwdfile" ]]; then
  exit 1
fi

if [[ ! -d "$cfgdir" ]]; then
  exit 1
fi


re_comment='^[[:space:]]*#.*$'
re_number='^[[:space:]]*[0-9]+[[:space:]]*$'
re_block='^[[:space:]]*\{[[:space:]]*[^{}#;]]*[^{}#]]*;[[:space::]]*\};[[:space:]]*$

is_valid_line(){
  line=$1
  echo "$line" | grep -Eq "$re_comment" && return 0
   # ако е число -> валиден
  echo "$line" | grep -Eq "$re_number" && return 0

  # ако е блок { ...; }; -> валиден
  echo "$line" | grep -Eq "$re_block" && return 0

  # иначе е невалиден
  return 1
}

# Функция: валидира цял файл
# Ако има грешки: печата ги и връща 1
# Ако е валиден: връща 0

validate_file() {
  file=$1
  lineno=0
  has_error=0
   # ще пазим грешките в променлива (за да ги отпечатаме накрая)
  
  errors=""
WHILE IFS= read -r line; do
  lineno=$((lineno+1))
  if ! is_valid_line "$line";then
    has_error=1
    errors='${errors} Line ${lineno}:${line}"$'\n;
  fi
done < "$file"
if[[ $has_error -e1 1 ]];then
  echo "error in "$file:"
  echo -n "$errors"
  return 1
fi
return 0
}
# 4) Взимаме всички .cfg файлове в cfgdir/ (рекурсивно)
tmp_list=$(mktemp)
find "$cfgdir" -type f -name '*.cfg' > "$tmp_list"
# 5) Обхождаме всеки cfg файл
WHILE IFS= read -r file:do
  # ако редът е празен (примерно find върне нищо), прескачаме
  [[ -z "$file" ]] && continue
# 5.1) Валидирай файла
  if [[validate_file "$file"]];then
   # 5.2) Ако е валиден -> добави съдържанието му към config.cf
    cat "$file">>"$outcfg"
     filebase=$(basename "$file")
    username="${filebase%.cfg}"
    
    # 5.4) ако username не съществува в pwdfile -> добавяме
    # Търсим ред, който започва с "username:"

  if [[! grep -q "^${username}:" "$pwdfile"]];then
     # 5.5) генерираме парола (16 символа, 1 парола)
     password=$(pwgen 16 1)

      # 5.6) md5 hash на паролата
      # echo -n -> без нов ред (иначе md5 ще е за "pass\n")
      passhash=$(echo -n "$password" | md5sum | cut -d ' ' -f1)
  # 5.7) добавяме "username:hash" към foo.pwd
      echo "${username}:${passhash}" >> "$pwdfile"

      # 5.8) печатаме username и паролата на STDOUT
      echo "${username} ${password}"
    fi
  fi

done < "$tmp_list"

  # 5.7) добавяме "username:hash" към foo.pwd
      echo "${username}:${passhash}" >> "$pwdfile"

      # 5.8) печатаме username и паролата на STDOUT
      echo "${username} ${password}"
    fi
  fi

done < "$tmp_list"
