#!/bin/bash

# 1) позволени са само oracle или grid
if [[ "$USER" != "oracle" && "$USER" != "grid" ]]; then
  echo "Error: this script can run only as oracle or grid" >&2
  exit 1
fi

# 2) 1 аргумент - часове
if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <hours>=2.." >&2
  exit 1
fi

HOURS="$1"

# 3) проверка да е цяло число (само цифри)
case "$HOURS" in
  ''|*[!0-9]*)
    echo "Error: hours must be an integer number" >&2
    exit 1
    ;;
esac

# 4) минимално 2 часа
if (( HOURS < 2 )); then
  echo "Error: minimum is 2 hours" >&2
  exit 1
fi

# 5) ORACLE_HOME трябва да е зададен
if [[ -z "${ORACLE_HOME:-}" ]]; then
  echo "Error: ORACLE_HOME is not set" >&2
  exit 1
fi

# 6) adrci трябва да съществува и да е изпълним
ADRCI="${ORACLE_HOME}/bin/adrci"
if [[ ! -x "$ADRCI" ]]; then
  echo "Error: adrci not found or not executable: $ADRCI" >&2
  exit 1
fi

# 7) diag_dest за потребителя
DIAG_DEST="/u01/app/$USER"
if [[ ! -d "$DIAG_DEST" ]]; then
  echo "Error: diag_dest directory does not exist: $DIAG_DEST" >&2
  exit 1
fi

# 8) превръщаме часове в минути (PURGE работи с минути)
AGE_MIN=$(( HOURS * 60 ))

# 9) взимаме списък с ADR homes
OUT="$("$ADRCI" exec="SET BASE $DIAG_DEST; SHOW HOMES")"

# 10) ако няма homes - край
echo "$OUT" | grep -q "No ADR homes are set"
if [[ $? -eq 0 ]]; then
  echo "No ADR homes are set for $USER"
  exit 0
fi

# 11) минаваме през редовете след "ADR Homes:"
FOUND=0
echo "$OUT" | while IFS= read -r line; do
  if [[ "$line" == "ADR Homes:" ]]; then
    FOUND=1
    continue
  fi

  # преди секцията "ADR Homes:" - игнорираме
  if [[ $FOUND -eq 0 ]]; then
    continue
  fi

  # празни редове - игнорираме
  if [[ -z "$line" ]]; then
    continue
  fi

  HOME="$line"

  # 12) взимаме името на 2-ро ниво директория: diag/<SECOND>/...
  SECOND="$(echo "$HOME" | cut -d'/' -f2)"

  # 13) интересни са само тези SECOND директории:
  case "$SECOND" in
    crs|tnslsnr|kfod|asm|rdbms)
      echo "Purging in home: $HOME (age ${AGE_MIN} minutes)"

      # 14) чистим безопасните файлове по-стари от AGE_MIN
      "$ADRCI" exec="SET BASE $DIAG_DEST; SET HOMEPATH $HOME; PURGE -AGE $AGE_MIN" >/dev/null
      ;;
    *)
      # не е интересен home -> пропускаме
      ;;
  esac
done
