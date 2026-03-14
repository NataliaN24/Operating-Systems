#!/bin/bash

if [[ $# -lt 1 ]]; then
    echo "At least one directory must be given" >&2
    exit 1
fi

if [[ -z "$REGISTRY_FILE" ]]; then
    echo "REGISTRY_FILE is not set" >&2
    exit 2
fi

if [[ -z "$REPORTS_DIR" ]]; then
    echo "REPORTS_DIR is not set" >&2
    exit 3
fi

if [[ ! -f "$REGISTRY_FILE" ]]; then
    touch "$REGISTRY_FILE"
fi

if [[ ! -d "$REPORTS_DIR" ]]; then
    mkdir -p "$REPORTS_DIR"
fi

for dir in "$@"; do
    if [[ ! -d "$dir" ]]; then
        echo "$dir is not a directory" >&2
        exit 4
    fi
done

timestamp=$(date +'%Y-%m-%d-%H-%M-%S')
reportFile="$REPORTS_DIR/$timestamp.report"

newFile=$(mktemp)
unchangedFile=$(mktemp)
modifiedFile=$(mktemp)
newRegistry=$(mktemp)

while read -r file; do
    absPath=$(realpath "$file")
    currentShaSum=$(sha256sum "$absPath" | cut -d ' ' -f1)

    oldLine=$(grep -F " $absPath" "$REGISTRY_FILE")

    if [[ -z "$oldLine" ]]; then
        echo "$absPath" >> "$newFile"
    else
        oldShaSum=$(echo "$oldLine" | cut -d ' ' -f1)

        if [[ "$currentShaSum" == "$oldShaSum" ]]; then
            echo "$absPath" >> "$unchangedFile"
        else
            echo "$absPath" >> "$modifiedFile"
        fi
    fi

    echo "$currentShaSum $absPath" >> "$newRegistry"
done < <(find "$@" -type f)

echo "new:" > "$reportFile"
cat "$newFile" >> "$reportFile"
echo >> "$reportFile"

echo "unchanged:" >> "$reportFile"
cat "$unchangedFile" >> "$reportFile"
echo >> "$reportFile"

echo "modified:" >> "$reportFile"
cat "$modifiedFile" >> "$reportFile"

mv "$newRegistry" "$REGISTRY_FILE"

rm -f "$newFile" "$unchangedFile" "$modifiedFile"
