Описание на формат на CSV (текстови) файл:
• CSV файлът представлява таблица, като всеки ред на таблицата е записан на отделен ред;
• на даден ред всяко поле (колона) е разделено от останалите със запетая;
• в полетата не може да присъства запетая, т.е. запетаята винаги е разделител между полета;
• броят на полетата на всеки ред е константа;
• първият ред във файла e header, който описва имената на колоните.
Разполагате с два CSV файла със следното примерно съдържание:
• base.csv:
unit name,unit symbol,measure
second,s,time
metre,m,length
kilogram,kg,mass
ampere,A,electric current
kelvin,K,thermodynamic temperature
mole,mol,amount of substance
candela,cd,luminous intensity
• prefix.csv:
prefix name,prefix symbol,decimal
tera,T,1000000000000
giga,G,1000000000
mega,M,1000000
mili,m,0.001
nano,n,0.000000001
Където смисълът на колоните е:
• за base.csv
– unit name – име на мерна единица
– unit symbol – съкратено означение на мерната единица
– measure – величина
• за prefix.csv
– prefix name – име на представка
– prefix symbol – означение на представка
– decimal – стойност
Забележка: Във файловете може да има и други редове, освен показаните в примера. Приемаме, че
файловете спазват описания по-горе формат, т.е. не е необходимо да проверявате формата.
Напишете shell скрипт, който приема три задължителни параметъра: число, prefix symbol и unit symbol.
Скриптът, ако може, трябва да извежда числото в основната мерна единица без представка, добавяйки
в скоби величината и името на мерната единица.
Примерен вход и изход:
$ ./foo.sh 2.1 M s
2100000.0 s (time, second)
Забележка: За изчисления може да ползвате bc.

#!/bin/bash

if [[ $# -ne 3 ]]; then
    echo "Usage: $0 <number> <prefix_symbol> <unit_symbol>"
    exit 1
fi

NUMBER="$1"
PREFIX="$2"
UNIT="$3"

BASE_CSV="base.csv"
PREFIX_CSV="prefix.csv"

DECIMAL=$(awk -F, -v ps="$PREFIX" 'NR>1 && $2==ps {print $3}' "$PREFIX_CSV")

if [[ -z "$DECIMAL" ]]; then
    echo "Unknown prefix symbol: $PREFIX"
    exit 1
fi


UNIT_INFO=$(awk -F, -v us="$UNIT" 'NR>1 && $2==us {print $3 "," $1}' "$BASE_CSV")

if [[ -z "$UNIT_INFO" ]]; then
    echo "Unknown unit symbol: $UNIT"
    exit 1
fi

MEASURE=$(echo "$UNIT_INFO" | cut -d, -f1)
UNIT_NAME=$(echo "$UNIT_INFO" | cut -d, -f2)

RESULT=$(echo "$NUMBER * $DECIMAL" | bc -l)

RESULT_FMT=$(printf "%.1f" "$RESULT")

echo "$RESULT_FMT $UNIT ($MEASURE, $UNIT_NAME)"

