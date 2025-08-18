#За удобство приемаме, че разполагате със системен инструмент sha256sum, който
#приема аргументи имена на файлове като за всеки файл пресмята и извежда уникална хеш стойност,
#базирана на съдържанието на файла. Изходът от инструмента е текстови, по един ред за всеки
#подаден като аргумент файл, в следния формат:
#• хеш стойност с дължина точно 64 знака
#• два интервала
#• име на файл
#Примерна употреба и изход:
#$ sha256sum /var/log/syslog /var/log/user.log README.md
#b2ff8bd882a501f71a144b7c678e3a6bc6764ac48eb1876fb5d11aac11014b78 /var/log/syslog
#e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855 /var/log/user.log
#e4702d8044b7020af5129fc69d77115fd4306715bd678ba4bef518b2edf01fb9 README.md
#Напишете скрипт, който приема задължителен параметър име на директория (ДИР1). Някъде в
#директорията ДИР1 може да съществуват архивни файлове с имена NAME_report-TIMESTAMP.tgz,
#където:
#• NAME е низ, който не съдържа символ ’_’
#• TIMESTAMP е във формат Unix time (POSIX time/UNIX Epoch time)
#На всяко пускане на скрипта се обработват само новосъздадените или модифицираните по съдържание спрямо предното
#пускане на скрипта архивни файлове от горния тип. За всеки такъв архивен
#файл се изпълнява следното:
#• ако архивният файл съдържа файл с име meow.txt, то този текстови файл да бъде записан под
#името /extracted/NAME_TIMESTAMP.txt, където NAME и TIMESTAMP са съответните стойности
#от името на архивния файл.

#/bin/bash
if [[ $# -ne 1 ]]; then
    echo "Usage: $0 DIR1"
    exit 1
fi

DIR1="$1"
STATEFILE="$HOME/.processed_archives"

mkdir -p /extracted

touch "$STATEFILE"

# 1. Търсим файлове с шаблон NAME_report-TIMESTAMP.tgz
find "$DIR1" -type f -regex '.*/[^_/]\+_report-[0-9]\+\.tgz' | while read -r archive; do
    # 2. Генерираме sha256sum
    hash=$(sha256sum "$archive" | awk '{print $1}')

    # 3. Проверяваме дали този хеш вече е обработен
    if grep -q "$hash  $archive" "$STATEFILE"; then
        continue  # вече е обработен и не е променен
    fi

    # 4. Вадим NAME и TIMESTAMP от името
    filename=$(basename "$archive")
    NAME="${filename%%_report-*}"
    TIMESTAMP=$(echo "$filename" | sed -E 's/.*_report-([0-9]+)\.tgz/\1/')

    # 5. Проверка за meow.txt вътре
    if tar -tzf "$archive" | grep -q '^meow.txt$'; then
        tar -xOzf "$archive" meow.txt > "/extracted/${NAME}_${TIMESTAMP}.txt"
        echo "Extracted meow.txt from $archive -> /extracted/${NAME}_${TIMESTAMP}.txt"
    fi

    # 6. Обновяваме state файла (махаме стари записи за този archive, ако има)
    grep -v "  $archive$" "$STATEFILE" > "$STATEFILE.tmp"
    echo "$hash  $archive" >> "$STATEFILE.tmp"
    mv "$STATEFILE.tmp" "$STATEFILE"
done

#or
#!/bin/bash

if [[ ${#} -ne 1 ]] ; then
        echo "Expected 1 argument - directory"
        exit 1
fi

DIR="${1}"

allArchives=$(find "${DIR}" -mindepth 1 -maxdepth 1 -type f -name "*_report-*.tgz" -printf "%p %M@\n")
lastExecution=$(stat -c "%X" ${0})
resultArchives=$(echo "${allArchives}" | awk -v var=${lastExecution} '{if ($2>=var) print $1}')

while read file ; do
        NAME=$(echo ${file} | sed -E 's/(.*)_/\1/')
        TIMESTAMP=$(echo ${file} | sed -E 's/report-(.*)[.]tgz/\1/')
        while read line ; do
                bn=$(basename ${line})
                if [[ ${bn} == "meow.txt" ]] && [[ -f ${bn} ]] ; then
                        tar -xf ${file} ${line}
                        dir=$(dirname ${file})
                        mv "${dir}/${line}" "/extracted/${NAME}_${TIMESTAMP}.txt"
                        break
                fi
        done < <(tar -tf ${file} | egrep "meow.txt")
done < <(echo ${resultArchive})


