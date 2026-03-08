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
