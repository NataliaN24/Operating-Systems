//a)
#!/bin/bash

if [[ ! -d "$ALLOWED_DIR" ]]; then
    echo "ALLOWED_DIR does not exist" >&2
    exit 1
fi

tmpfile=$(mktemp)

for arg in "$@"; do

    result=$(find "$ALLOWED_DIR" -path "$arg")

    if [[ -z "$result" ]]; then
        echo "Error: cannot remove object outside ALLOWED_DIR" >&2
        rm "$tmpfile"
        exit 1
    fi

    echo "$arg" >> "$tmpfile"

done

xargs /usr/bin/rm < "$tmpfile"

rm "$tmpfile"

//b) !!!!
#!/bin/bash

dir="$1"

if [[ ! -d "$dir" ]]; then
    exit 1
fi

dir=$(realpath "$dir")
fake=$(realpath "./fake-commands")

ALLOWED_DIR="$dir" PATH="$fake:$PATH" bash




/////////////////////////////////
#!/bin/bash

if [[ "$#" -ne 1 ]]; then
    exit 1
fi

if [[ ! -d "$1" ]]; then
    exit 2
fi

ALLOWED_DIR=$(realpath "$1")
SCRIPT_DIR=$(dirname "$(realpath "$0")")

tmp=$(mktemp)

# записваме PATH по един елемент на ред
echo "$PATH" | tr ':' '\n' > "$tmp"

newtmp=$(mktemp)

# създаваме нов PATH файл
while read -r dir; do

    if [[ "$dir" == "/usr/bin" ]]; then
        echo "$SCRIPT_DIR/fake-commands" >> "$newtmp"
        echo "$dir" >> "$newtmp"
    else
        echo "$dir" >> "$newtmp"
    fi

done < "$tmp"

# заменяме стария файл с новия
mv "$newtmp" "$tmp"

# правим PATH обратно от файла
newpath=$(tr '\n' ':' < "$tmp")
newpath=${newpath%:}

rm "$tmp"
///noooooo
export ALLOWED_DIR
export PATH="$newpath"

bash -i
