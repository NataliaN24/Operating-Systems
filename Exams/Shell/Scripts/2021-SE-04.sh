Разполагате с машина, на която е инсталиран специализиран софтуер, който ползва два потребителски акаунта – oracle и grid. За всеки от двата акаунта съществува директория, която ще наричаме
diag_dest и тя е от вида /u01/app/потребител.
Всеки от потребителите би трябвало да има environment променлива ORACLE_HOME, която указва абсолютен път до директория във формат /път/до/дир. В поддиректорията bin на зададената директория
би трябвало да има изпълним файл с име adrci.
Всеки от потребителите може да подаде серия от подкоманди, които неговото копие на adrci да изпълни, като го извика с параметър exec="cmd1; cmd2; cmd3". Отделните подкоманди се разделят с
точка и запетая (;). Командата adrci винаги приключва с exit code 0.
Нека дефинираме следните подкоманди:
• SET BASE – за да се гарантира правилна работа на командата adrci, при всяко нейно извикване
първата подкоманда трябва да бъде от вида SET BASE diag_dest, където diag_dest е абсолютният път на съответната директория за дадения потребител
• SHOW HOMES – подкоманда SHOW HOMES извежда на STDOUT един от следните два изхода:
– вариант 1: (неуспех): No ADR homes are set
– вариант 2: (успех):
ADR Homes:
diag/rdbms/orclbi/orclbi1
diag/rdbms/orclbi/orclbi2
Ако командата се изпълни успешно, тя връща списък с един или повече ADR Homes, които са
релативни имена на директории спрямо diag_dest на съответният потребител.
От полученият списък с релативни пътища интересни за нас са само тези, които за име на директория на второ ниво имат една от следните директории: crs, tnslsnr, kfod, asm или rdbms.
• SET HOMEPATH – подкоманда SET HOMEPATH път задава активна работна директория, спрямо която ще се изпълняват последващи подкоманди в рамките на същото изпълнение на adrci; път
е релативен път до директория, спрямо изхода на SHOW HOMES
• PURGE – подкоманда PURGE -AGE минути изтрива определени файлове в текущо активната работна директория, по-стари от дефинираното време в минути. Забележка: изтриват се само
безопасни файлове, т.е. такива, чието изтриване няма да доведе до проблем. Дефиницията на
безопасни файлове е извън обхвата на тази задача.
Напишете shell скрипт, който може да се изпълнява само от някой от указаните два акаунта и приема
задължителен първи позиционен аргумент число (в часове, минимална стойност 2 часа). Скриптът
трябва да почиства безопасните файлове по-стари от зададените часове в интересните ADR Home-ове
на съответния потребител.

#!/bin/bash

if [[ "$(whoami)" != "oracle" && "$(whoami)" != "grid" ]]; then
    echo "This script can only be run by oracle or grid users."
    exit 1
fi

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <hours>"
    exit 1
fi

HOURS=$1

if [[ $HOURS -lt 2 ]]; then
    echo "Error: Minimum hours is 2"
    exit 1
fi

if [[ -z "$ORACLE_HOME" ]]; then
    echo "Error: ORACLE_HOME is not set"
    exit 1
fi

ADRCI="$ORACLE_HOME/bin/adrci"
if [[ ! -x "$ADRCI" ]]; then
    echo "Error: $ADRCI not found or not executable"
    exit 1
fi

OUTPUT=$("$ADRCI" exec="set base $(dirname $ORACLE_HOME)/diag; show homes")
if echo "$OUTPUT" | grep -q "No ADR homes are set"; then
    echo "No ADR homes are set for user $(whoami)"
    exit 0
fi

INTEREST=("crs" "tnslsnr" "kfod" "asm" "rdbms")

while read -r LINE; do

    [[ "$LINE" == "ADR Homes:"* ]] && continue
  
    DIRNAME=$(echo "$LINE" | cut -d/ -f2)
    for I in "${INTEREST[@]}"; do
        if [[ "$DIRNAME" == "$I" ]]; then
            
            "$ADRCI" exec="set base $(dirname $ORACLE_HOME)/diag; set homepath $LINE; purge -age $((HOURS*60))"
        fi
    done
done <<< "$(echo "$OUTPUT" | tail -n +2)"

echo "Cleanup done."

