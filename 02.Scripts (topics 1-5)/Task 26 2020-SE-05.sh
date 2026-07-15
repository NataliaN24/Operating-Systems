#!/bin/bash

pwd="$1"
config="$2"
dir="$3"

# временен файл за валидните cfg файлове
validFile=$(mktemp)

find "$dir" -type f -name "*.cfg" -print0 | while IFS= read -r -d '' file
do
    errors=$(mktemp)

    count=1
    found=0

    while IFS= read -r line
    do
        # коментар
        if [[ "$line" =~ ^# ]]; then
            count=$((count+1))
            continue
        fi

        # валиден ред
        if ! echo "$line" | grep -Eq '^\{ [a-zA-Z-]+ \};$'
        then
            found=1
            echo "Line $count:$line" >> "$errors"
        fi

        count=$((count+1))

    done < "$file"


    # ако има грешки
    if [[ "$found" -eq 1 ]]
    then
        echo "Error in $file:"
        cat "$errors"

    else
        # добавяме към общия config
        cat "$file" >> "$validFile"


        # име без .cfg = потребител
        username=$(basename "$file" .cfg)


        # проверка дали го има в pwd файла
        if ! grep -q "^$username:" "$pwd"
        then
            # генериране на парола
            password=$(pwgen 16 1)

            # md5 на паролата
            hash=$(echo -n "$password" | md5sum | cut -d' ' -f1)

            echo "$username:$hash" >> "$pwd"

            echo "$username $password"
        fi
    fi

done

# запис на крайния config
mv "$validFile" "$config"
//////////////////////////////////////////////////////////////////////////////
#!/bin/bash

if [[ $# -ne 3 ]]; then
    echo "Usage: $0 foo.pwd config.cfg cfgdir"
    exit 1
fi

pwdfile="$1"
config="$2"
cfgdir="$3"

if [[ ! -f "$pwdfile" ]]; then
    echo "Invalid password file"
    exit 1
fi

if [[ ! -d "$cfgdir" ]]; then
    echo "Invalid cfg directory"
    exit 1
fi

> "$config"

find "$cfgdir" -type f -name "*.cfg" | while read -r file; do

    invalid=0
    line_num=0

    while read -r line; do
        line_num=$((line_num + 1))

        if ! echo "$line" | grep -Eq '^(#.*|\{ [^}]+ \};)$'; then
            if [[ $invalid -eq 0 ]]; then
                echo "Error in $(basename "$file"):"
                invalid=1
            fi

            echo "Line $line_num:$line"
        fi

    done < "$file"

    if [[ $invalid -eq 0 ]]; then
        cat "$file" >> "$config"

        username=$(basename "$file" .cfg)

        if ! grep -q "^${username}:" "$pwdfile"; then
            password=$(pwgen 16 1)
            hash=$(echo -n "$password" | md5sum | cut -d ' ' -f1)

            echo "${username}:${hash}" >> "$pwdfile"
            echo "$username $password"
        fi
    fi

done
