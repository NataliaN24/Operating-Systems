#!/bin/bash
# Скриптът е направен по ТВОЯТА логика, но:
# - без awk
# - оправя "първият запис" (прескача празни редове и редове-коментари)
# - оправя проверката за SOA токен (без \b, което не е portable в grep -E)
# - брои SOA коректно (игнорира коментари след ';')
# - вади serial и при едноредов, и при многоредов SOA
# - увеличава TT с bash arithmetic (без bc) и пази водеща нула
# - заменя само ПЪРВОТО срещане на serial (по условие то е уникално, но така е по-безопасно)
#
# Грешките са лесни за grep:  ERROR:<file>: <message>

if [[ $# -eq 0 ]]; then
  echo "ERROR: no input files" >&2
  exit 1
fi

# Днешна дата YYYYMMDD
currDate="$(date +%Y%m%d)"

for file in "$@"; do
  # 0) Проверки за файл
  if [[ ! -f "$file" ]]; then
    echo "ERROR:${file}: not a regular file" >&2
    continue
  fi
  if [[ ! -r "$file" || ! -w "$file" ]]; then
    echo "ERROR:${file}: need read+write permissions" >&2
    continue
  fi

  # 1) Намираме ПЪРВИЯ "реален" запис, не просто първия ред:
  #    - пропускаме празни редове
  #    - пропускаме редове, които са само коментар (след whitespace започват с ';')
  firstLine="$(grep -m1 -vE '^[[:space:]]*($|;)' "$file")"

  if [[ -z "$firstLine" ]]; then
    echo "ERROR:${file}: empty or comment-only file" >&2
    continue
  fi

  # Махаме коментара от първия запис, за да не ни пречи на regex-ите
  firstLineNoComment="$(echo "$firstLine" | sed -E 's/[[:space:]]*;.*$//')"

  # 2) Проверка: първият запис трябва да е SOA (SOA като отделен токен)
  # (не ползваме \b, защото grep -E не гарантира word-boundary)
  if ! echo "$firstLineNoComment" | grep -Eq '(^|[[:space:]])SOA($|[[:space:]])'; then
    echo "ERROR:${file}: first record must be SOA" >&2
    continue
  fi

  # 3) Проверка: точно 1 SOA в целия файл
  # - първо режем коментарите след ';'
  # - после търсим SOA като отделен токен
  soaCount="$(
    sed -E 's/[[:space:]]*;.*$//' "$file" |
    grep -E '(^|[[:space:]])SOA($|[[:space:]])' |
    wc -l
  )"

  if [[ "$soaCount" -ne 1 ]]; then
    echo "ERROR:${file}: expected exactly 1 SOA record, found ${soaCount}" >&2
    continue
  fi

  # 4) Вадим serial-а:
  #    (A) пробваме да го извадим от първия запис (едноредов SOA)
  #        SOA RDATA започва с: <mname-fqdn> <rname-fqdn> <serial>
  #        => търсим 10 цифри след SOA + 2 FQDN-а
  serial="$(
    echo "$firstLineNoComment" |
      sed -nE 's/.*[[:space:]]SOA[[:space:]]+[a-z0-9.]+\.[[:space:]]+[a-z0-9.]+\.[[:space:]]+([0-9]{10}).*/\1/p'
  )"

  #    (B) ако не е намерен, приемаме многоредов SOA:
  #        serial е първото 10-цифрено число след първия запис (след реда със SOA ... '(' )
  if [[ -z "$serial" ]]; then
    # Вземаме номера на реда на първия реален запис (SOA) за да започнем "след него"
    firstLineNo="$(grep -n -m1 -vE '^[[:space:]]*($|;)' "$file" | cut -d':' -f1)"

    # Взимаме от следващия ред нататък, режем коментари, и хващаме първото 10-цифрено число
    serial="$(
      tail -n +"$((firstLineNo + 1))" "$file" |
        sed -E 's/[[:space:]]*;.*$//' |
        grep -m1 -oE '[0-9]{10}'
    )"
  fi

  if [[ -z "$serial" ]]; then
    echo "ERROR:${file}: cannot find SOA serial (10 digits YYYYMMDDTT)" >&2
    continue
  fi

  # 5) Алгоритъм за нов serial
  fileDate="${serial:0:8}"   # YYYYMMDD
  tt="${serial:8:2}"         # TT

  # Ако fileDate е в бъдеще -> не пипаме (за да не "намалим" serial по грешка)
  if [[ "$fileDate" > "$currDate" ]]; then
    echo "ERROR:${file}: serial date ${fileDate} is in the future (today ${currDate})" >&2
    continue
  fi

  if [[ "$fileDate" < "$currDate" ]]; then
    newSerial="${currDate}00"
  else
    # fileDate == currDate -> увеличаваме TT
    if [[ "$tt" == "99" ]]; then
      echo "ERROR:${file}: TT is 99 for today; cannot increment" >&2
      continue
    fi

    # 10#$tt -> казваме "вземи го като десетична стойност" (важно при водеща нула)
    ttNum=$((10#$tt + 1))
    newSerial="${currDate}$(printf "%02d" "$ttNum")"
  fi

  # 6) Подмяна: заменяме само първото срещане на serial-а
  # (GNU sed: "0,/<pat>/ s//repl/" сменя само първото срещане)
  if ! sed -E -i "0,/${serial}/s//${newSerial}/" "$file"; then
    echo "ERROR:${file}: failed to update file with sed" >&2
    continue
  fi

done

exit 0
