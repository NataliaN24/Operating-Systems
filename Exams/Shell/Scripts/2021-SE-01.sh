Разполагате с машина, на която е инсталиран специализиран софтуер, който ползва два потребителски акаунта – oracle и grid. Всеки от потребителите би трябвало да има environment променлива
ORACLE_HOME, която указва абсолютен път до директория във формат /path/to/dir. В под-директория
bin на зададената директория би трябвало да има изпълним файл с име adrci. Всеки от двата потребителя има собствена директория diag_dest, която е във вида /u01/app/потребител. Когато някой от
потребителите изпълни неговото копие на командата adrci с параметър exec="show homes" може
да получи на STDOUT един от следните два изхода:
• вариант 1: (неуспех): No ADR homes are set
• вариант 2: (успех):
ADR Homes:
diag/rdbms/orclbi/orclbi1
diag/rdbms/orclbi/orclbi2
И в двата случая командата приключва с exit code 0. Ако командата се изпълни успешно, тя връща
списък с един или повече ADR Homes, които са релативни имена на директории спрямо diag_dest на
съответният потребител.
Напишете скрипт, който може да се изпълнява само от някой от тези два акаунта, и извежда на STDOUT
размера в мегабайти и абсолютният път на всеки ADR Home.
Примерен изход:
0 /u01/app/oracle/diag/rdbms/orclbi/orclbi1
389 /u01/app/oracle/diag/rdbms/orclbi/orclbi2

#!/bin/bash

USER=$(whoami)
if [[ "$USER" != "oracle" && "$USER" != "grid" ]]; then
    echo "This script can only be run by oracle or grid users"
    exit 1
fi

if [[ -z "$ORACLE_HOME" ]]; then
    echo "ORACLE_HOME is not set"
    exit 1
fi

ADR_BIN="$ORACLE_HOME/bin/adrci"
if [[ ! -x "$ADR_BIN" ]]; then
    echo "adrci executable not found in $ADR_BIN"
    exit 1
fi

DIAG_DEST="/u01/app/$USER"

OUTPUT=$($ADR_BIN exec="show homes")

if echo "$OUTPUT" | grep -q "No ADR homes are set"; then
    echo "No ADR homes found"
    exit 0
fi

# Пропускаме първия ред ("ADR Homes:") и обхождаме останалите
echo "$OUTPUT" | tail -n +2 | while IFS= read -r home; do
    # Почистваме водещи/крайни интервали
    clean_home=$(echo "$home" | awk '{$1=$1; print}')
    full_path="$DIAG_DEST/$clean_home"
    # Размер в MB
    if [[ -d "$full_path" ]]; then
        size=$(du -sm "$full_path" | awk '{print $1}')
    else
        size=0
    fi
    echo "$size $full_path"
done
