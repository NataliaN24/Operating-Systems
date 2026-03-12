#!/bin/bash

# Проверяваме дали има точно един аргумент (файл със спецификация)
if [[ $# -ne 1 ]]; then
    echo "error" >&2
    exit 2
fi

# Четем входния файл ред по ред
while read -r line
do

    # Вземаме името на файла/директорията (първото нещо в реда)
    filename=$(echo "${line}" | grep -E -o "^(/.*)+")

    # Вземаме типа на обекта: file / dir / symlink / nonexistant
    filetype=$(echo "${line}" | grep -E -o "(dir|file|symlink|nonexistant)")

    # Вземаме owner:group ако съществува
    user=$(echo "${line}" | grep -E -o "\b.*:.*\b")

    # Вземаме permission mask (напр. 0755)
    permMask=$(echo "${line}" | grep -E -o "[0-9]{3,4}")

    # ------------------------------------------------------
    # Обработка на symbolic link
    # ------------------------------------------------------

    if [[ "${filetype}" == "symlink" ]]; then

        # target към който трябва да сочи линкът
        dest=$(echo "$line" | cut -d ' ' -f 3)

        # проверяваме дали файлът съществува
        found=$(find "${filename}" 2>/dev/null)

        # ако не съществува -> създаваме symlink
        if [[ -z "${found}" ]]; then
            ln -s "${dest}" "${filename}"
            continue
        fi
    fi


    # ------------------------------------------------------
    # Проверка дали файлът съществува
    # ------------------------------------------------------

    found=$(find "${filename}" 2>/dev/null)

    # ако файлът съществува, но трябва да не съществува
    if [[ -n "${found}" && "$filetype" == "nonexistant" ]]; then
        rm -r "$filename"
        continue
    fi


    # ------------------------------------------------------
    # Ако файлът НЕ съществува
    # ------------------------------------------------------

    if [[ -z "$found" ]]; then

        # ако трябва да не съществува -> нищо не правим
        if [[ "${filetype}" == "nonexistant" ]]; then
            continue
        else

            # ако трябва да е директория
            if [[ "${filetype}" == "dir" ]]; then
                mkdir "$filename"
                chmod "$permMask" "$filename"

            # ако трябва да е обикновен файл
            else
                touch "$filename"
                chmod "$permMask" "$filename"
            fi
        fi


    # ------------------------------------------------------
    # Ако файлът вече съществува
    # ------------------------------------------------------

    else

        # Проверяваме типа на съществуващия файл
        currentType=$(stat "$filename" -c "%F")

        # ако типът съвпада -> само обновяваме permissions
        if [[ "${filetype}" == "file" && "$currentType" == "regular file" ]]; then
            chmod "$permMask" "$filename"

        elif [[ "${filetype}" == "dir" && "$currentType" == "directory" ]]; then
            chmod "$permMask" "$filename"

        # ако типът е грешен -> изтриваме и създаваме наново
        else

            rm -r "$filename"

            if [[ "${filetype}" == "dir" ]]; then
                mkdir "$filename"
                chmod "$permMask" "$filename"
            else
                touch "$filename"
                chmod "$permMask" "$filename"
            fi
        fi
    fi


    # ------------------------------------------------------
    # Ако има owner:group -> променяме собствеността
    # ------------------------------------------------------

    if [[ -n "$user" ]]; then
        chown "$user" "$filename"
    fi

# Четем от подадения файл
done < "${1}"

exit 0
