Името на дадена машина можете да вземете с командата hostname -s.
Разполагате с машина, на която е инсталиран специализиран софтуер, който ползва два потребителски акаунта – oracle и grid.
22
Всеки от потребителите би трябвало да има environment променлива ORACLE_BASE, която указва абсолютен път до директория във формат /път/до/дир.
Всеки от потребителите би трябвало да има environment променлива ORACLE_HOME, която указва абсолютен път до директория във формат /път/до/дир. В поддиректорията bin на зададената директория
би трябвало да има изпълним файл с име sqlplus.
Всеки от потребителите би трябвало да има environment променлива ORACLE_SID с някакъв низ като
стойност.
Ако горните три environment променливи съществуват, всеки от потребителите може да изпълнява
неговото копие на командата sqlplus със следните параметри: sqlplus -SL "/ as роля" @foo.sql
където роля трябва да бъде низът SYSDBA при изпълнение от oracle и SYSASM при изпълнение от grid.
И в двата случая sqlplus изпълнява SQL заявките от файла (foo.sql, името на файла няма значение)
и извежда изхода на stdout. Ако съдържанието на sql файла е:
SELECT value FROM v$parameter WHERE name = 'diagnostic_dest';
EXIT;
изходът ще бъде стойността на търсения параметър diagnostic_dest в следния вид:
oracle@astero:~$ sqlplus -SL "/ as sysdba" @a.sql
VALUE
--------------------------------------------------------------------------------
/u01/app/oracle
oracle@astero:~$
Параметърът diagnostic_dest може да няма стойност, в който случай изведеният низ ще е празен.
Изходът винаги е 5 реда, стойността винаги е на 4-и ред. Ако командата sqlplus не се изпълни успешно, тя ще върне ненулев exit code.
За всеки от двата акаунта съществува директория, която ще наричаме diag_base. Конкретната директория е:
• същата като ORACLE_BASE, ако diagnostic_dest няма стойност
• същата като diagnostic_dest, ако diagnostic_dest има стойност
За всеки от двата акаунта би трябвало да съществува под-директория на diag_base с име diag, която
ще наричаме diag_dir.
Съществуват три множества интересни за нас файлове:
• множество crs – за потребител grid, в diag_dir има под-директория crs, в която има под-директория
с името на машината, в която има под-директория crs, в която има под-директория trace. Интересни за нас файлове в тази директория са такива, чието име завършва на подчертавка-число
и имат разширение trc или trm, например foo_356.trc, bar_40001.trm.
• множество tnslsnr – за потребител grid, в diag_dir има под-директория tnslsnr, в която има
под-директория с името на машината, в която има няколко директории с различни имена. Във
всяка от тези директории има под-директории alert и trace. Интересни за нас са файловете
в alert, чието име завършва на подчертавка-число и имат разширение xml (напр. baz_78.xml)
и файловете в trace, чието име завършва на подчертавка-число и имат разширение log (напр.
qux_88231.log).
• множество rdbms – за потребител oracle, в diag_dir има под-директория rdbms, в която има няколко под-директории, във всяка от които може да има няколко под-директории. Интересни за
нас са само файловете в тези директории на второ ниво, чието име завършва на подчертавкачисло и имат разширение trc или trm, например corge_27.trc, grault_1024.trm.
Напишете скрипт, който може да се изпълнява само от някой от тези два акаунта, и приема задължителен първи позиционен аргумент число (в дни). В зависимост от това кой потребител изпълнява
скрипта, той трябва да извежда на stdout за всяко множество на съответния потребител общият размер (в килобайти) на описаните по-горе интересни файлове за които времето на последната промяна
(по съдържание) на файла е по-голямо от зададеното като параметър на скрипта.
23
Примерно изпълнение и изход:
oracle@astero:~$ ./foo.sh 42
rdbms: 14400
grid@astero:~$ ./foo.sh 73
crs: 28800

#!/bin/bash


if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <days>"
    exit 1
fi

DAYS="$1"
USER="$(whoami)"
HOST="$(hostname -s)"

get_diag_base() {
    local ROLE="$1"
    local ORACLE_BASE="$2"
    local ORACLE_HOME="$3"
    local ORACLE_SID="$4"
    local SQL_FILE="/tmp/diag.sql"

    echo "SELECT value FROM v\$parameter WHERE name='diagnostic_dest';" > "$SQL_FILE"
    echo "EXIT;" >> "$SQL_FILE"

    if [[ "$ROLE" == "oracle" ]]; then
        ROLE_SQL="SYSDBA"
    else
        ROLE_SQL="SYSASM"
    fi

    DIAG_DEST=$($ORACLE_HOME/bin/sqlplus -SL "/ as $ROLE_SQL" @"$SQL_FILE" 2>/dev/null | sed -n '4p')

    rm -f "$SQL_FILE"

    if [[ -z "$DIAG_DEST" ]]; then
        echo "$ORACLE_BASE"
    else
        echo "$DIAG_DEST"
    fi
}

if [[ "$USER" != "oracle" && "$USER" != "grid" ]]; then
    echo "Script can only be run by oracle or grid"
    exit 1
fi

: "${ORACLE_BASE:?}"
: "${ORACLE_HOME:?}"
: "${ORACLE_SID:?}"

DIAG_BASE=$(get_diag_base "$USER" "$ORACLE_BASE" "$ORACLE_HOME" "$ORACLE_SID")
DIAG_DIR="$DIAG_BASE/diag"

if [[ "$USER" == "oracle" ]]; then
    SUM=$(find "$DIAG_DIR/rdbms" -mindepth 2 -maxdepth 2 -type f \( -name "*_*.trc" -o -name "*_*.trm" \) -mtime -"$DAYS" -exec du -k {} + | awk '{sum+=$1} END{print sum}')
    echo "rdbms: ${SUM:-0}"
else
    SUM_CRS=$(find "$DIAG_DIR/crs/$HOST/crs/trace" -type f \( -name "*_*.trc" -o -name "*_*.trm" \) -mtime -"$DAYS" -exec du -k {} + | awk '{sum+=$1} END{print sum}')
    echo "crs: ${SUM_CRS:-0}"

    SUM_TNS=$(find "$DIAG_DIR/tnslsnr/$HOST" -type f \( -path "*/alert/*_*.xml" -o -path "*/trace/*_*.log" \) -mtime -"$DAYS" -exec du -k {} + | awk '{sum+=$1} END{print sum}')
    echo "tnslsnr: ${SUM_TNS:-0}"
fi


tnslsnr: 33600
Забележка: Правилното ползване на временни файлове е разрешено
