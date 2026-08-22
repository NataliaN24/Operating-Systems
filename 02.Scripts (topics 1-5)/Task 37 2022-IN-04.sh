#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <fuga_dir>" >&2
    exit 1
fi

fuga="$1"

if [[ ! -d "$fuga" ]]; then
    echo "Error: $fuga is not a directory" >&2
    exit 1
fi

if [[ ! -f "$fuga/foo.pwd" ]]; then
    echo "Error: missing foo.pwd" >&2
    exit 1
fi

if [[ ! -x "$fuga/validate.sh" ]]; then
    echo "Error: missing or non-executable validate.sh" >&2
    exit 1
fi

if [[ ! -d "$fuga/cfg" ]]; then
    echo "Error: missing cfg directory" >&2
    exit 1
fi

tmp_conf=$(mktemp)

find "$fuga/cfg" -type f -name '*.cfg' | while read -r file; do
    output=$("$fuga/validate.sh" "$file")
    code=$?

    if [[ "$code" -eq 0 ]]; then
        cat "$file" >> "$tmp_conf"

        username=$(basename "$file")
        username="${username%.cfg}"

        if ! grep -q "^$username:" "$fuga/foo.pwd"; then
            password=$(pwgen 16 1)
            hash=$(mkpasswd "$password")

            echo "$username:$hash" >> "$fuga/foo.pwd"
            echo "$username:$password"
        fi

    elif [[ "$code" -eq 1 ]]; then
        echo "$output" | while read -r line; do
            echo "$file:$line" >&2
        done

    else
        echo "$file: validation script error" >&2
    fi
done

cat "$tmp_conf" > "$fuga/foo.conf"
rm "$tmp_conf"
