#!/bin/bash

# 1) Проверки за аргументи
if [[ $# -ne 2 ]]; then
  exit 1
fi

src=$1
dst=$2

# 2) SRC трябва да е директория
if [[ ! -d "$src" ]]; then
  exit 1
fi

if [[ -e "$dst" ]]; then
  exit 1
fi

mkdir -p "$dst/images" "$dst/by-date" "$dst/by-album" "$dst/by-title"

# -------- помощни функции --------

# Почиства низ: trim + collapse multiple spaces -> single space

clean() 
{
  # trim: маха leading/trailing spaces
  # collapse: превръща поредици от spaces в един space
   echo "$1" | sed -E 's/^[[:space:]]+//;s/[[:space:]]+$//;s/[[:space:]]+/ /g'
}

# Взима последния елемент в скоби
get_album(){
  raw="$1"
  last=$(echo "$raw" | grep -oE '\([^)]*\' | tail -n 1)

 if [[ -z "$last"]];then
    echo "misc"
    return 
fi

last=$(echo "$last" | sed 's/^(\(.*\))$/\1/' #1 means to replace it with what's inside the brackets
last=$(clean "$last")

if [[ -z "$last"]];then
  echo "misc"
else
  echo "$last"
fi
}
# Махаме всички (...) и чистим
get_title()
{
  raw="$1"
  title=$(echo "$raw" | sed -E 's/([^)]*)//'
  clean "$title"
}
get_date()
{
  file="$1"
  stat -c '%y' "$file" | cut -d ' ' -f1
}

get_hash(){
  file="$1"
  sha256sum "$file" | cut -c1-16
}

while IFS= read -r -d '' file;do
  base=$(basename "$file")
  raw="${base%.jpg}"
  title=$(get_title,"$raw")
  album=$(get_album,"$raw")
  datev=$(get_date "$file")
  hash=$(get_hash16 "$file")

  img_path="$dst/images/$hash.jpg"
  # копираме файла
  cp "$file" "$img_path"
  
   # генерираме 5-те линка
  link1="$dst/by-date/$datev/by-album/$album/by-title/$title.jpg"
  link2="$dst/by-date/$datev/by-title/$title.jpg"
  link3="$dst/by-album/$album/by-date/$datev/by-title/$title.jpg"
  link4="$dst/by-album/$album/by-title/$title.jpg"
  link5="$dst/by-title/$title.jpg"

  for link in "$link1" "$link2" "$link3" "$link4" "$link5"; do
    link_dir=$(dirname "$link")
    mkdir -p "$link_dir"
    
     # относителен target
    target_rel=$(realpath --relative-to="$link_dir" "$img_path")

    ln -sf "$target_rel" "$link"
  done
done < <(find "$src" -type f -name '*.jpg' -print0)

