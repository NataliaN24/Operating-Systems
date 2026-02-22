#first script

tmp=$(mktemp)
grep -E '^-?[0-9]+$' > "$tmp"

maxAbs=0
while IFS= read -r n;do
  abs=$(echo "$n" | sed 's/^-//')   #remove the minus
  if [[ abs > maxAbs ]];then
    maxAbs="$abs"
  fi
  done < "$tmp"
  while IFS= read -r n;do
    abs=$(echo "$n" | sed 's/^-//')
     if (( abs == maxAbs )); then
    echo "$n"
  fi
done < "$tmp" | sort -n | uniq



#second script
#!/bin/bash
set -euo pipefail

tmp="$(mktemp)"
trap 'rm -f "$tmp"' EXIT

# 1) Вземаме само редове, които са цели числа
grep -E '^-?[0-9]+$' > "$tmp" || true
[[ -s "$tmp" ]] || exit 0

# 2) Намираме maxSum = максималната сума на цифрите
maxSum=0
while IFS= read -r n; do
  abs=$(echo "$n" | sed 's/^-//')

  # Превръщаме "123" -> "1\n2\n3" -> "1+2+3" -> 6
  sum=$(echo "$abs" \
        | sed 's/./&\n/g' \
        | paste -sd+ - \
        | bc)

  if (( sum > maxSum )); then
    maxSum="$sum"
  fi
done < "$tmp"

# 3) Печатаме всички числа със sumDigits == maxSum,
#    после взимаме най-малкото уникално
while IFS= read -r n; do
  abs=$(echo "$n" | sed 's/^-//')

  sum=$(echo "$abs" \
        | sed 's/./&\n/g' \
        | paste -sd+ - \
        | bc)

  if (( sum == maxSum )); then
    echo "$n"
  fi
done < "$tmp" | sort -n | uniq | head -n 1
