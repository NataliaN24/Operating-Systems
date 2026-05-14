#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 <project_dir> <output_svg>"
    exit 1
fi

dir="$1"
out="$2"

if [[ ! -d "$dir" ]]; then
    echo "First argument must be a directory"
    exit 1
fi

graph=$(mktemp)

find "$dir" -type f \( -name "*.h" -o -name "*.hh" -o -name "*.hpp" \) | while read -r file; do

    grep '^class ' "$file" | while read -r line; do

        class=$(echo "$line" | tr -s ' ' | cut -d ' ' -f2)

        echo "$class" >> "$graph"

        if echo "$line" | grep ':' > /dev/null; then

            inheritance=$(echo "$line" | cut -d ':' -f2-)

            echo "$inheritance" | tr ',' '\n' | while read -r parent_part; do

                parent=$(echo "$parent_part" | sed 's/^ *//' | tr -s ' ' | cut -d ' ' -f2)

                if [[ -n "$parent" ]]; then
                    echo "$parent -> $class" >> "$graph"
                fi

            done
        fi

    done

done

dag-ger "$graph" > "$out"

rm "$graph"


##################################################################################################################

#!/bin/bash

if [[ ${#} -ne 2 ]];
then
    echo "error"
    exit 1
fi

classes=$(mktemp)

while read -r header;
do
    decl=$(grep -E "^class[ ]+[a-zA-Z_][a-zA-Z0-9_]*([ ]*:[ ]*(private|protected|public)[ ]+[a-zA-Z_][a-zA-Z0-9_]*([ ]*,[ ]*(private|protected|public)[ ]+[a-zA-Z_][a-zA-Z0-9_]*)*)?$" "$header")

    if [[ -z "$decl" ]]; then
        continue
    fi

    className=$(echo "$decl" | cut -d ':' -f 1 | cut -d ' ' -f 2)
    echo "$className" >> "$classes"

    doesDerives=$(echo "$decl" | grep -o ":")

    if [[ "$doesDerives" == ":" ]];
    then
        descs=$(echo "$decl" | cut -d ':' -f 2 | tr ',' '\n')

        while read -r part;
        do
            parent=$(echo "$part" | sed -E 's/^[ ]*(private|protected|public)[ ]+//')

            if [[ -n "$parent" ]]; then
                echo "$parent" >> "$classes"
                echo "$parent -> $className" >> "$classes"
            fi
        done <<< "$descs"
    fi
done < <(find "$1" -type f -name "*.h")

sort -u "$classes" | dag-ger > "$2"

rm -f "$classes"
exit 0
