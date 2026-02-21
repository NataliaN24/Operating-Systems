#!/bin/bash

if [[ $# -lt 1 || $# -gt 2 ]]; then
  echo "Usage: $0 DIR [OUTFILE]" >&2
  exit 1
fi

dir="$1"
out="$2"

if [[ ! -d "$dir" ]]; then
  echo "Error: not a directory" >&2
  exit 1
fi

# ако има out файл -> append към него, иначе stdout
if [[ -n "$out" ]]; then
  exec >> "$out"
fi

broken=0

find "$dir" -type l -print0 |
while IFS= read -r -d '' link; do
  # ако target съществува
  if [[ -e "$link" ]]; then
    target="$(readlink "$link")"
    printf '%s -> %s\n' "$link" "$target"
  else
    broken=$((broken + 1))
  fi
done

# ВАЖНО: broken няма да се промени извън pipe-а (subshell)!
# Затова броим счупените с отделен find:
broken=$(find "$dir" -type l ! -exec test -e {} \; -print | wc -l)

printf 'Broken symlinks: %s\n' "$broken"
