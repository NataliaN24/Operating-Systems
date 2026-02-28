#!/bin/bash

# 1) позволени потребители
user="$(whoami)"
if [[ "$user" != "oracle" && "$user" != "grid" ]]; then
  exit 1
fi

# 2) ORACLE_HOME проверки
if [[ -z "${ORACLE_HOME:-}" ]]; then
  exit 1
fi

# да е абсолютен път
if [[ "${ORACLE_HOME}" != /* ]]; then
  exit 1
fi

if [[ ! -d "$ORACLE_HOME" ]]; then
  exit 1
fi

adrci="$ORACLE_HOME/bin/adrci"
if [[ ! -x "$adrci" ]]; then
  exit 1
fi

# 3) diag_dest
diag_dest="/u01/app/$user"

if [[ ! -d "$diag_dest" ]]; then
  exit 1
fi

# 4) взимаме homes
out="$("$adrci" exec="show homes")"

# вариант 1: няма homes
if echo "$out" | grep -q '^No ADR homes are set$'; then
  exit 0
fi

# 5) изкарваме редовете след "ADR Homes:" и ги обработваме
# Първо махаме всичко до "ADR Homes:" включително, после махаме празните редове.
homes="$(echo "$out" | sed -n '/^ADR Homes:$/,$p' | sed '1d' | grep -v '^[[:space:]]*$')"

# ако по някаква причина няма редове след ADR Homes:
if [[ -z "$homes" ]]; then
  exit 0
fi

# 6) за всеки home -> absolute path + du -sm
# (чете ред по ред, безопасно за интервали - тук няма интервали по условие)
while IFS= read -r home; do
  abs="$diag_dest/$home"

  # ако директорията липсва, просто я пропускаме (или можеш да exit 1)
  if [[ ! -d "$abs" ]]; then
    continue
  fi

  mb="$(du -sm "$abs" | cut -f1)"
  echo "$mb $abs"
done <<< "$homes"

