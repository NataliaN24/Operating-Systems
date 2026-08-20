#!/bin/bash

dir="$1"

if [[ ! -d "$dir" ]]; then
    exit 1
fi

symLinks=$(mktemp)

find "$dir" -type l | while read -r file; do

    target=$(readlink "$file")

    echo "$file $target" >> "$symLinks"

done

while read -r file target; do

    targetPath=$(realpath "$(dirname "$file")/$target")
    dirPath=$(realpath "$dir")

    if [[ "$targetPath" == "$dirPath/"* ]]; then

        path=$(realpath --relative-to="$(dirname "$file")" "$targetPath")

    else

        path="$targetPath"

    fi

    rm "$file"
    ln -s "$path" "$file"

done < "$symLinks"

rm "$symLinks"
//////////////////////////////////////////////
#!/bin/bash

if [[ "$#" -ne 1 ]]; then
    exit 1
fi

if [[ ! -d "$1" ]]; then
    exit 2
fi

dir=$(realpath "$1")

while read -r link; do

    # Пълен път до обекта, към който сочи връзката
    target=$(readlink -f "$link")

    # Директорията, в която се намира самият symbolic link
    link_dir=$(dirname "$link")

    # Проверяваме дали target е вътре в подадената директория
    if [[ "$target" == "$dir"/* ]]; then
        
        # Връзката трябва да бъде relative
        newtarget=$(realpath --relative-to="$link_dir" "$target")

    else
        
        # Връзката трябва да бъде absolute
        newtarget="$target"

    fi

    # Изтриваме старата връзка
    rm "$link"

    # Създаваме новата връзка
    ln -s "$newtarget" "$link"

done < <(find "$1" -type l)
