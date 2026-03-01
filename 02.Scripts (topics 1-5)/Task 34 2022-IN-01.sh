#!/bin/bash

if [[ $# -ne 2 ]]; then
  echo "Usage: $0 <dir1> <dir2>" >&2
  exit 1
fi

 dir1="${1}"
 dir2="${2}"

# dir1 трябва да е директория
if [[ ! -d "$dir1" ]]; then
  echo "Error: dir1 is not a directory" >&2
  exit 1
fi

# dir2 трябва да е директория
if [[ ! -d "$dir2" ]]; then
  echo "Error: dir2 is not a directory" >&2
  exit 1
fi

# dir2 трябва да е празна
if find "$dir2" -mindepth 1 -print -quit | grep -q .; then
  echo "Error: dir2 must be empty" >&2
  exit 1
fi

find "$dir1" -type f ! -name '.*.swp' | while IFS= read -r f; do
  # правим пътя относителен спрямо dir1
  rel="${f#"$dir1"/}"

  # създаваме нужната поддиректория в dir2
  mkdir -p "$dir2/$(dirname "$rel")"

  # копираме файла на съответното място
  cp "$f" "$dir2/$rel"
done
