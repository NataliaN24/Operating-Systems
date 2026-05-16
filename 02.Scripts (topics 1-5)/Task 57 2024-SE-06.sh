#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <config-file>" >&2
    exit 1
fi

config="$1"

if [[ ! -f "$config" ]]; then
    echo "Config file does not exist" >&2
    exit 1
fi

while read -r line; do

    [[ -z "$line" ]] && continue

    path=$(echo "$line" | cut -d ' ' -f1)
    type=$(echo "$line" | cut -d ' ' -f2)

    ########################################
    # FILE
    ########################################
    if [[ "$type" == "file" ]]; then

        third=$(echo "$line" | cut -d ' ' -f3)
        fourth=$(echo "$line" | cut -d ' ' -f4)

        hasOwner=false
        owner=""
        permissions=""

        if [[ "$third" == *:* ]]; then
            hasOwner=true
            owner="$third"
            permissions="$fourth"
        else
            permissions="$third"
        fi

        # exists but wrong type
        if [[ -e "$path" && ! -f "$path" ]]; then
            rm -rf "$path" || {
                echo "Cannot remove $path" >&2
                continue
            }
        fi

        # create if missing
        if [[ ! -e "$path" ]]; then
            mkdir -p "$(dirname "$path")" || {
                echo "Cannot create parent directory for $path" >&2
                continue
            }

            touch "$path" || {
                echo "Cannot create file $path" >&2
                continue
            }
        fi

        currentPerm=$(stat -c '%a' "$path")

        if [[ "$currentPerm" != "$permissions" ]]; then
            chmod "$permissions" "$path" || \
                echo "Cannot chmod $path" >&2
        fi

        if [[ "$hasOwner" == "true" ]]; then
            currentOwner=$(stat -c '%U:%G' "$path")

            if [[ "$currentOwner" != "$owner" ]]; then
                chown "$owner" "$path" || \
                    echo "Cannot chown $path" >&2
            fi
        fi

    ########################################
    # DIRECTORY
    ########################################
    elif [[ "$type" == "dir" ]]; then

        third=$(echo "$line" | cut -d ' ' -f3)
        fourth=$(echo "$line" | cut -d ' ' -f4)

        hasOwner=false
        owner=""
        permissions=""

        if [[ "$third" == *:* ]]; then
            hasOwner=true
            owner="$third"
            permissions="$fourth"
        else
            permissions="$third"
        fi

        # exists but wrong type
        if [[ -e "$path" && ! -d "$path" ]]; then
            rm -rf "$path" || {
                echo "Cannot remove $path" >&2
                continue
            }
        fi

        # create if missing
        if [[ ! -e "$path" ]]; then
            mkdir -p "$path" || {
                echo "Cannot create directory $path" >&2
                continue
            }
        fi

        currentPerm=$(stat -c '%a' "$path")

        if [[ "$currentPerm" != "$permissions" ]]; then
            chmod "$permissions" "$path" || \
                echo "Cannot chmod $path" >&2
        fi

        if [[ "$hasOwner" == "true" ]]; then
            currentOwner=$(stat -c '%U:%G' "$path")

            if [[ "$currentOwner" != "$owner" ]]; then
                chown "$owner" "$path" || \
                    echo "Cannot chown $path" >&2
            fi
        fi

    ########################################
    # SYMLINK
    ########################################
    elif [[ "$type" == "symlink" ]]; then

        target=$(echo "$line" | cut -d ' ' -f3)

        recreate=false

        if [[ ! -L "$path" ]]; then
            recreate=true
        else
            currentTarget=$(readlink "$path")

            if [[ "$currentTarget" != "$target" ]]; then
                recreate=true
            fi
        fi

        if [[ "$recreate" == "true" ]]; then

            if [[ -e "$path" || -L "$path" ]]; then
                rm -rf "$path" || {
                    echo "Cannot remove $path" >&2
                    continue
                }
            fi

            mkdir -p "$(dirname "$path")" || {
                echo "Cannot create parent directory for $path" >&2
                continue
            }

            ln -s "$target" "$path" || \
                echo "Cannot create symlink $path" >&2
        fi

    ########################################
    # NONEXISTANT
    ########################################
    elif [[ "$type" == "nonexistant" ]]; then

        if [[ -e "$path" || -L "$path" ]]; then
            rm -rf "$path" || \
                echo "Cannot remove $path" >&2
        fi
    fi

done < "$config"
#############################################################################################################################################

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
