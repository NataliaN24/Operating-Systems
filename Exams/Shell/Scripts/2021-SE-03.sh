#Напишете shell скрипт, който приема два позиционни параметъра – имена на файлове. Примерно извикване:
#$ ./foo.sh input.bin output.h
#Файлът input.bin е двоичен файл с елементи uint16_t числа, създаден на little-endian машина.
#Вашият скрипт трябва да генерира C хедър файл, дефиниращ масив с име arr, който:
#• съдържа всички елементи от входния файл;
#• няма указана големина;
#• не позволява промяна на данните.
#Генерираният хедър файл трябва да:
#• съдържа и uint32_t променлива arrN, която указва големината на масива;
#• бъде валиден и да може да се #include-ва без проблеми от C файлове, очакващи да “виждат”
#arr и arrN.
#За да е валиден един входен файл, той трябва да съдържа не повече от 524288 елемента.
#За справка, dump на съдържанието на примерен input.bin:
#00000000: 5555 5655 5955 5a55 6555 6655 6955 6a55 UUVUYUZUeUfUiUjU
#00000010: 9555 9655 9955 9a55 a555 a655 a955 aa55 .U.U.U.U.U.U.U.U

#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 <input.bin> <output.h>"
    exit 1
fi

IN="$1"
OUT="$2"

NUM_ELEMS=$(( $(stat -c%s "$IN") / 2 ))
MAX_ELEMS=524288

if [[ $NUM_ELEMS -gt $MAX_ELEMS ]]; then
    echo "Error: input file contains more than $MAX_ELEMS elements"
    exit 1
fi

{
echo "#ifndef ARR_HEADER_H"
echo "#define ARR_HEADER_H"
echo ""
echo "#include <stdint.h>"
echo ""
echo "const uint32_t arrN = $NUM_ELEMS;"
echo "const uint16_t arr[] = {"


xxd -e -g2 "$IN" | awk '{for(i=2;i<=NF;i++) printf "0x%s, ", $i}'

echo ""
echo "};"
echo ""
echo "#endif /* ARR_HEADER_H */"

} > "$OUT"

echo "Header file '$OUT' generated with $NUM_ELEMS elements."
