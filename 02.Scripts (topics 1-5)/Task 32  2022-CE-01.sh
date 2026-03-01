#!/bin/bash

if [[$# -ne 3 ]]; then
  exit 1
fi

num=${1}
prefixSym=${2}
unitSym=${3}

unitFile="base.csv"
prefixFile="prefix.csv"

if [[ ! -f "$unitFile" || ! -f "$prefixFile" ]]; then
  exit 2
fi

multiplier=$(grep ",$prefixSym," "$prefixFile" | cut -d',' -f3)
if [[ -z "$multiplier" ]]; then
  echo "Prefix not found"
  exit 3
fi

num=(( $multipier * $num)
unitName=$(grep ",$unitSym," "$unitFile" | cut -d',' -f1)
measure=$(grep ",$unitSym," "$unitFile" | cut -d',' -f3)

if [[ -z "$unitName" ]]; then
  echo "Unit not found"
  exit 4
fi

# 5) изчисление с bc (защото може да има десетично число)
result=$(echo "$num * $multiplier" | bc -l)

# 6) изход
echo "$result $unitSym ($measure, $unitName)"
