#!/bin/bash

dir="$1"

if [[ ! -d "$dir" ]]; then
    exit 1
fi

initialState=$(df -B1 "$dir" | tail -1 | tr -s ' ' | cut -d ' ' -f4)

allFiles=$(mktemp)

find "$dir" -type f -exec sha256sum {} \; > "$allFiles"

deduplicated=0

while read -r hash file; do

    filesWithTheSameHash=$(grep "^$hash " "$allFiles")
    howMany=$(echo "$filesWithTheSameHash" | wc -l)

    if [[ "$howMany" -eq 1 ]]; then
        continue
    fi

    target="$file"
    deduplicated=$((deduplicated + 1))

    while read -r h f; do

        if [[ "$target" == "$f" ]]; then
            continue
        fi

        rm "$f"
        ln "$target" "$f"

    done <<< "$filesWithTheSameHash"

    # махаме обработените файлове от списъка,
    # за да не обработим същата група отново
    sed -i "/^$hash /d" "$allFiles"

done < <(cat "$allFiles")

currentState=$(df -B1 "$dir" | tail -1 | tr -s ' ' | cut -d ' ' -f4)

freed=$((currentState - initialState))

echo "Deduplicated groups: $deduplicated"
echo "Freed bytes: $freed"

rm "$allFiles"

//////////////////////////////////////////////////
#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

dir="$1"

if [[ ! -d "$dir" ]]; then
    exit 2
fi

allFiles=$(mktemp)
filesHash=$(mktemp)
hashes=$(mktemp)

find "$dir" -type f > "$allFiles"

while read -r file; do

    hash=$(sha256sum "$file" | cut -d ' ' -f1)
    inode=$(stat -c '%i' "$file")
    size=$(stat -c '%s' "$file")

    echo "$file $hash $inode $size" >> "$filesHash"

done < "$allFiles"


# Get every different hash
cut -d ' ' -f2 "$filesHash" | sort -u > "$hashes"

groups=0
freed=0


while read -r hash; do

    sameFiles=$(mktemp)

    grep " $hash " "$filesHash" > "$sameFiles"

    count=$(wc -l < "$sameFiles")

    # Only groups containing at least two files
    if (( count > 1 )); then

        # Check whether all files already point
        # to the same inode.
        inodeCount=$(cut -d ' ' -f3 "$sameFiles" | sort -u | wc -l)

        if (( inodeCount > 1 )); then

            groups=$((groups + 1))

            # Keep the first file
            first=$(head -n 1 "$sameFiles" | cut -d ' ' -f1)

            # Size of one copy
            size=$(head -n 1 "$sameFiles" | cut -d ' ' -f4)

            # Every other separate copy can be removed
            freed=$((freed + (count - 1) * size))

            tail -n +2 "$sameFiles" |
            while read -r file hash inode size; do

                rm "$file"
                ln "$first" "$file"

            done
        fi
    fi

    rm -f "$sameFiles"

done < "$hashes"


echo "Deduplicated groups: $groups"
echo "Freed space: $freed bytes"

rm -f "$allFiles" "$filesHash" "$hashes"

/////////////////////////////////////////////////////
#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

if [[ ! -d "$1" ]]; then
    exit 2
fi

dir="$1"
tmp=$(mktemp)

find "$dir" -type f | while read -r file; do
    hash=$(sha256sum "$file" | cut -d ' ' -f1)
    size=$(stat -c '%s' "$file")
    inode=$(stat -c '%i' "$file")

    echo "$hash;$size;$inode;$file" >> "$tmp"
done

dedup_groups=0
freed_bytes=0

for hash in $(cut -d ';' -f1 "$tmp" | sort | uniq); do
    count=$(grep "^$hash;" "$tmp" | wc -l)

    if [[ "$count" -le 1 ]]; then
        continue
    fi

    inode_count=$(grep "^$hash;" "$tmp" | cut -d ';' -f3 | sort | uniq | wc -l)

    if [[ "$inode_count" -le 1 ]]; then
        continue
    fi

    dedup_groups=$((dedup_groups + 1))

    first_file=$(grep "^$hash;" "$tmp" | head -n 1 | cut -d ';' -f4)
    size=$(grep "^$hash;" "$tmp" | head -n 1 | cut -d ';' -f2)

    grep "^$hash;" "$tmp" | tail -n +2 | while IFS=';' read -r h s ino file; do
        if [[ "$file" != "$first_file" ]]; then
            rm "$file"
            ln "$first_file" "$file"
        fi
    done

    freed_bytes=$((freed_bytes + (inode_count - 1) * size))
done

echo "Deduplicated groups: $dedup_groups"
echo "Freed bytes: $freed_bytes"

rm "$tmp"
################################################################################################33



#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <dir>" >&2
    exit 1
fi

dir="$1"

if [[ ! -d "$dir" ]]; then
    echo "Not a directory" >&2
    exit 1
fi

tmp=$(mktemp)

find "$dir" -type f | while read -r file; do
    hash=$(sha256sum "$file" | cut -d ' ' -f1)
    size=$(stat -c '%s' "$file")
    echo "$hash $size $file"
done | sort > "$tmp"

dedup_groups=0
freed=0

cut -d ' ' -f1 "$tmp" | sort | uniq | while read -r hash; do

    count=$(grep "^$hash " "$tmp" | wc -l)

    if [[ "$count" -ge 2 ]]; then
        dedup_groups=$((dedup_groups + 1))

        first=$(grep "^$hash " "$tmp" | head -n 1 | cut -d ' ' -f3-)
        size=$(grep "^$hash " "$tmp" | head -n 1 | cut -d ' ' -f2)

        freed=$((freed + size * (count - 1)))

        grep "^$hash " "$tmp" | tail -n +2 | while read -r h s file; do
            rm -- "$file"
            ln -- "$first" "$file"
        done
    fi

done

echo "Deduplicated $dedup_groups groups."
echo "Freed $freed bytes."

rm -f "$tmp"

########################################################################################################################################33
#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <dir>"
    exit 1
fi

if [[ ! -d "$1" ]]; then
    echo "The argument must be a directory"
    exit 1
fi

dir="$1"

tmp=$(mktemp)

while IFS= read -r file;do
    hash=$(sha256sum "$file" | cut -d ' ' -f1)
    size=$(stat -c '%s' "$file")
    echo "$hash|$size|$file" >> "$tmp"
done< < (find "$1" -mindepth 1 -type f)

sorted=$(mktemp)
sort "$tmp" > "$sorted"
currentHash="" 
currentSize=0
count=0
original=""
duplicates=()  #array
groups=0;
saved=0;

while IFS='|' read -r hash size file;do
  if [[ "$hash" =="currentHash"]];then
    count=$((count+1))
    duplicates+=("$file")
  else
    if [[ $count -ge 2]];then
      groups=$((groups+1))
      saved=$((saved +(count-1 ) *currentSize))

      for duplicates in "${duplicates[@]}";do
      rm -f "$duplicates"
      ln "$original" "$duplicate"
      done
    fi
  currentHash="$hash"
  currentSize="$size"
  count=1;
  original="$file"
  duplicates=()
  
done< "$sorted"

if [[ $count -ge 2 ]]; then
    groups=$((groups + 1))
    saved=$((saved + (count - 1) * current_size))

    for duplicate in "${duplicates[@]}"; do
        rm -f "$duplicate"
        ln "$original" "$duplicate"
    done
fi

echo "Deduplicated groups: $groups"
echo "Saved bytes: $saved"

rm -f "$tmp" "$sorted"
