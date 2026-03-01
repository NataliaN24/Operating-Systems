#!/bin/bash
if [[ ${#} -ne 2 ]] ; then
  echo "Expected 2 arguments" >&2
  exit 1
fi

BINARY="${1}"
OUTPUT="${2}"

if [[ ! -f "${BINARY}" ]] ; then
  echo "File does not exist" >&2
  exit 1
fi

# size in bytes
SIZE_BYTES=$(stat -c '%s' "${BINARY}")

# must be multiple of 2 (uint16_t)
if (( SIZE_BYTES % 2 != 0 )); then
  echo "Invalid input: size must be multiple of 2 bytes (uint16_t elements)" >&2
  exit 1
fi

# number of uint16_t elements
N=(( SIZE_BYTES / 2 ))
# max 524288 elements
if (( N > 524288 )); then
  echo "Invalid input: too many elements (${N}), max is 524288" >&2
  exit 1
fi

touch ${OUTPUT}
echo "#include <stdint.h>" > ${OUTPUT}
ARR=""

WHILE read BYTE; do
   # BYTE е 4 hex символа (2 байта), например: 3412
    # обръщаме байтовете (little endian -> правилен ред)
    
  FLIPPED=$(eho "$BYTE" | sed 's/^(..\)\(..\)$/\2\1/')
   # добавяме към масива
  ARR="${ARR}0${FLIPPED}"
done< < (xxd $BINARY | cut -d ' ' -f2-9)

ARR=${ARR%,}
echo "uint32_t arr[] = { ${ARR} };" > ${OUTPUT}
