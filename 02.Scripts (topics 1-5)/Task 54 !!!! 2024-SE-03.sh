#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 N" >&2
    exit 1
fi

N="$1"

notes=("A" "Bb" "B" "C" "Db" "D" "Eb" "E" "F" "Gb" "G" "Ab")

while read line; do
    result=""

    while [[ "$line" == *"["*"]"* ]]; do
        before=$(echo "$line" | cut -d '[' -f1)
        after_bracket=$(echo "$line" | cut -d '[' -f2-)

        chord=$(echo "$after_bracket" | cut -d ']' -f1)
        after=$(echo "$after_bracket" | cut -d ']' -f2-)

        second=$(echo "$chord" | cut -c2)

        if [[ "$second" == "b" ]]; then
            tone=$(echo "$chord" | cut -c1-2)
            rest=$(echo "$chord" | cut -c3-)
        else
            tone=$(echo "$chord" | cut -c1)
            rest=$(echo "$chord" | cut -c2-)
        fi

        index=-1

        for i in "${!notes[@]}"; do
            if [[ "${notes[$i]}" == "$tone" ]]; then
                index="$i"
            fi
        done

        if [[ "$index" -eq -1 ]]; then
            new_chord="$chord"
        else
            new_index=$(( (index + N) % 12 ))
            new_chord="${notes[$new_index]}$rest"
        fi

        result="$result$before[$new_chord]"
        line="$after"
    done

    echo "$result$line"
done
##############################################################################
#!/bin/bash

if [[ $# -ne 1 ]];
then
    echo "error"
    exit 1
fi

if ! echo "$1" | grep -Eq '^[0-9]+$';
then
    echo "error"
    exit 2
fi

N=$1

notes=("A" "Bb" "B" "C" "Db" "D" "Eb" "E" "F" "Gb" "G" "Ab")

transpose_note() {
    oldNote="$1"

    i=0
    while [[ $i -lt 12 ]];
    do
        if [[ "${notes[$i]}" == "$oldNote" ]];
        then
            newIndex=$(( (i + N) % 12 ))
            echo "${notes[$newIndex]}"
            return
        fi
        i=$(( i + 1 ))
    done

    echo "$oldNote"
}

while IFS= read -r line;
do
    while echo "$line" | grep -q '\[[^]]*\]';
    do
        chord=$(echo "$line" | grep -o '\[[^]]*\]' | head -n 1)
        inside=$(echo "$chord" | cut -c2- | rev | cut -c2- | rev)

        firstTwo=$(echo "$inside" | cut -c1-2)
        firstOne=$(echo "$inside" | cut -c1)

        if echo "$firstTwo" | grep -Eq '^(Bb|Db|Eb|Gb|Ab)$';
        then
            root="$firstTwo"
            rest=$(echo "$inside" | cut -c3-)
        else
            root="$firstOne"
            rest=$(echo "$inside" | cut -c2-)
        fi

        newRoot=$(transpose_note "$root")
        newChord="[$newRoot$rest]"

        line=$(echo "$line" | sed "s/\[[^]]*\]/$newChord/")
    done

    echo "$line"
done
