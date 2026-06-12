#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 input.bin output.h" >&2
    exit 1
fi

input="$1"
output="$2"

if [[ ! -f "$input" ]]; then
    echo "Error: $input is not a file" >&2
    exit 1
fi

size=$(stat -c '%s' "$input")

if [[ $((size % 2)) -ne 0 ]]; then
    echo "Error: invalid file size" >&2
    exit 1
fi

count=$((size / 2))

if [[ $count -gt 524288 ]]; then
    echo "Error: too many elements" >&2
    exit 1
fi

{
    echo '#include <stdint.h>'
    echo
    echo 'const uint16_t arr[] = {'

    xxd -p -c 2 "$input" | while read bytes; do
        b1=$(echo "$bytes" | cut -c 1-2)
        b2=$(echo "$bytes" | cut -c 3-4)

        echo "    0x$b2$b1,"
    done

    echo '};'
    echo
    echo "const uint32_t arrN = $count;"
} > "$output"
