#!/bin/bash

if [[ $# -ne 3 ]]; then
    exit 1
fi

src="${1}"
dst="${2}"
abc="${3}"
#must be root
if [[ "$(id -u)" -ne 0 ]]; then
 echo "Error: must be run as root" >&2
  exit 1
fi
#validate dirs

if [[ ! -d "$src" ]]; then
 echo "Error: SRC is not a directory" >&2
  exit 1
fi

if [[ ! -d "$dst" ]]; then
  echo "Error: DST is not a directory or dst is not empty" >&2
  exit 1
fi

if [[ -z "$(find "$src" -mindepth 1)" ]]; then 
  echo "The first directory must not be empty..." >&2
  exit 2
fi

if [[ -n "$(find "$dst" -mindepth 1)" ]]; then 
  echo "The 2nd directory must  be empty..." >&2
  exit 2
fi

# find files whose *name* contains abc, move preserving structure

find "$src" -type f -name "*$abc*" -print0 | 
while IFS= read -r -d '' file;do       #d stands for stop reading at null character
  #to remove the src prefix
  relPath=$(echo "$file" | sed "s|^$src/||")
  mkdir -p "$dst/$(dirname "$relpath")"
  mv "$file" "$dst/$relpath"
done

