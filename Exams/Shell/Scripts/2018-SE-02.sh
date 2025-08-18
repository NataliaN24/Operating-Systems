#Напишете скрипт, който приема два позиционни аргумента – име на текстови файл
#и директория. Директорията не трябва да съдържа обекти, а текстовият файл (US-ASCII)
#е стенограма и всеки ред е в следния формат:
#ИМЕ ФАМИЛИЯ (уточнения): Реплика
#където:
#• ИМЕ ФАМИЛИЯ присъстват задължително;
#• ИМЕ и ФАМИЛИЯ се състоят само от малки/главни латински букви и тирета;
#• (уточнения) не е задължително да присъстват;
#• двоеточието ‘:’ присъства задължително;
#• Репликата не съдържа знаци за нов ред;
#• в стринга преди двоеточието ‘:’ задължително има поне един интервал между ИМЕ и ФАМИЛИЯ;
#• наличието на други интервали където и да е на реда е недефинирано.
#Примерен входен файл:
#John Lennon (The Beatles): Time you enjoy wasting, was not wasted.
#Roger Waters: I’m in competition with myself and I’m losing.
#John Lennon:Reality leaves a lot to the imagination.
#Leonard Cohen:There is a crack in everything, that’s how the light gets in.
#Скриптът трябва да:
#• създава текстови файл dict.txt в посочената директория, който на всеки ред да съдържа:
#ИМЕ ФАМИЛИЯ;НОМЕР
#където:
#– ИМЕ ФАМИЛИЯ е уникален участник в стенограмата (без да се отчитат уточненията);
#– НОМЕР е уникален номер на този участник, избран от вас.
#• създава файл НОМЕР.txt в посочената директория, който съдържа всички (и само) редовете
#на дадения участник.

#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 transcript.txt output_dir"
    exit 1
fi

INPUT="$1"
OUTDIR="$2"
if [ ! -f "$INPUT" ]; then
    echo "Error: Input file does not exist."
    exit 1
fi

if [ ! -d "$OUTDIR" ]; then
    echo "Error: Output dir does not exist."
    exit 1
fi

if [ "$(ls -A "$OUTDIR")" ]; then
    echo "Error: Output dir is not empty."
    exit 1
fi

DICT="$OUTDIR/dict.txt"
touch "$DICT"

participant=$(awk -F ':' '{print $1}' "$INPUT" | awk '{print $1, $2}' | sort -u)

n=1
echo "$participants" | while read -r name; do
    echo "$name;$n" >> "$DICT"
    n=$((n+1))
done

# 3. Обхождаме редовете и пишем във файловете
while IFS= read -r line; do
    # взимаме име фамилия (първите 2 думи)
    fullname=$(echo "$line" | awk -F: '{print $1}' | awk '{print $1, $2}')

    # намираме съответния номер от dict.txt
    number=$(grep -F "$fullname;" "$DICT" | cut -d';' -f2)

    # записваме реда в правилния файл
    echo "$line" >> "$OUTDIR/$number.txt"
done < "$INPUT"
