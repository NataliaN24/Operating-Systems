 Напишете скрипт, който приема два аргумента - имена на директории. Първата (SRC)
съществува, докато втората (DST) трябва да бъде създадена от скрипта. Директорията SRC и нейните
поддиректории може да съдържат файлове, чиито имена завършат на .jpg. Имената на файловете
може да съдържат интервали, както и поднизове, оградени със скоби, например:
A single (very ugly) tree (Outdoor treks) 2.jpg
Falcons.jpg
Gorgonzola (cheese).jpg
Leeches (two different ones) (Outdoor treks).jpg
Pom Pom Pom.jpg
За даден низ ще казваме, че е почистен, ако от него са премахнати leading и trailing интервалите и
всички последователни интервали са сведени до един.
За всеки файл дефинираме следните атрибути:
• заглавие - частта от името преди .jpg, без елементи оградени в скоби, почистен. Примери:
A single tree 2
Falcons
Gorgonzola
Leeches
Pom Pom Pom
• албум - последният елемент от името, който е бил ограден в скоби, почистен. Ако албум е празен
стринг, ползваме стойност по подразбиране misc. Примери:
Outdoor treks
misc
cheese
Outdoor treks
misc
• дата - времето на последна модификация на съдържанието на файла, във формат YYYY-MMDD
• хеш - първите 16 символа от sha256 сумата на файла. Забележка: приемаме, че в тази идеална
вселена първите 16 символа от sha256 сумата са уникални за всеки файл от тези, които ще се
наложи да обработваме.
Скриптът трябва да създава в директория DST необходимата структура от под-директории, файлове
и symlink–ове, така че да са изпълнени следните условия за всеки файл от SRC:
• DST/images/хеш.jpg - копие на съответния файл
• следните обекти са относителни symlink–ове към хеш.jpg:
– DST/by-date/дата/by-album/албум/by-title/заглавие.jpg
– DST/by-date/дата/by-title/заглавие.jpg
– DST/by-album/албум/by-date/дата/by-title/заглавие.jpg
– DST/by-album/албум/by-title/заглавие.jpg
– DST/by-title/заглавие.jpg


#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 <SRC> <DST>"
    exit 1
fi

SRC="$1"
DST="$2"

if [[ ! -d "$SRC" ]]; then
    echo "Source directory $SRC does not exist"
    exit 1
fi

mkdir -p "$DST/images"

# функция за почистване на whitespace
clean() {
    echo "$1" | sed -E 's/^[[:space:]]+//; s/[[:space:]]+$//; s/[[:space:]]+/ /g'
}

# намираме всички jpg файлове
find "$SRC" -type f -name "*.jpg" | while IFS= read -r file; do
    base=$(basename "$file")
    name="${base%.jpg}"

    # извличаме всички скоби по прост начин
    albums=()
    temp="$name"
    while echo "$temp" | grep -q "("; do
        # взимаме първата скоба
        part=$(echo "$temp" | grep -o "(".*")" | head -n1)
        # махаме скобите
        content=$(echo "$part" | sed 's/(//; s/)//')
        albums+=("$(clean "$content")")
        # премахваме първата скоба от текста
        temp=$(echo "$temp" | sed "s/$(echo "$part" | sed 's/[[\.*^$/]/\\&/g')//")
    done

    # Албум = последният елемент, ако няма  misc
    if [[ ${#albums[@]} -eq 0 ]]; then
        album="misc"
    else
        album="${albums[-1]}"
    fi

    # Заглавие = остатъка след премахване на скоби
    title=$(clean "$temp")

    # Дата на последна модификация YYYY-MMDD
    date=$(date -r "$file" +%Y-%m%d)

    # Хеш (първите 16 символа)
    hash=$(sha256sum "$file" | cut -c1-16)

    dst_image="$DST/images/$hash.jpg"
    cp -n "$file" "$dst_image"

    mkdir -p "$DST/by-date/$date/by-album/$album"
    mkdir -p "$DST/by-date/$date/by-title"
    mkdir -p "$DST/by-album/$album/by-date/$date"
    mkdir -p "$DST/by-album/$album"
    mkdir -p "$DST/by-title"

    # Създаваме относителни symlink-ове
    ln -sf "../images/$hash.jpg" "$DST/by-date/$date/by-album/$album/$title.jpg"
    ln -sf "../../images/$hash.jpg" "$DST/by-date/$date/by-title/$title.jpg"
    ln -sf "../../../images/$hash.jpg" "$DST/by-album/$album/by-date/$date/$title.jpg"
    ln -sf "../../images/$hash.jpg" "$DST/by-album/$album/$title.jpg"
    ln -sf "images/$hash.jpg" "$DST/by-title/$title.jpg"
done
