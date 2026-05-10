#!/bin/bash
#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <dir>" >&2
    exit 1
fi

dir="$1"

if [[ ! -d "$dir" ]]; then
    echo "Error: not a directory" >&2
    exit 1
fi

tmp=$(mktemp)

find "$dir" -type f | while read -r file; do
    hash=$(sha256sum "$file" | cut -d ' ' -f1)
    inode=$(stat -c '%i' "$file")

    echo "$hash $inode $file" >> "$tmp"
done

cut -d ' ' -f1 "$tmp" | sort | uniq | while read -r hash; do

    files_for_hash=$(mktemp)

    grep "^$hash " "$tmp" > "$files_for_hash"

    hardlink_groups=$(mktemp)
    single_files=$(mktemp)

    cut -d ' ' -f2 "$files_for_hash" | sort | uniq | while read -r inode; do
        count=$(grep "^$hash $inode " "$files_for_hash" | wc -l)

        if [[ "$count" -gt 1 ]]; then
            echo "$inode" >> "$hardlink_groups"
        else
            grep "^$hash $inode " "$files_for_hash" | cut -d ' ' -f3- >> "$single_files"
        fi
    done

    if [[ -s "$hardlink_groups" ]]; then

        cat "$hardlink_groups" | while read -r inode; do
            grep "^$hash $inode " "$files_for_hash" |
            head -n 1 |
            cut -d ' ' -f3-
        done

        cat "$single_files"

    else
        count=$(cat "$single_files" | wc -l)

        if [[ "$count" -gt 1 ]]; then
            tail -n +2 "$single_files"
        fi
    fi

    rm "$files_for_hash" "$hardlink_groups" "$single_files"

done

rm "$tmp"
#################################################################################################
[[ $# -ne 1 ]] && echo "1 param expected" && exit 1
[[ ! -d $1 ]] && echo "the 1st param must be a dir" && exit 2

files=$(find $1 -type f -printf '%i %n %p\n')

DIR=$(mktemp -d)
while read inode count name; do
	sha=$(sha256sum $name | cut -d ' ' -f 1)

	echo "$inode $count" >> $DIR/$sha
done < <(echo "$files")

files=$(find $DIR -type f)

while read sha; do
	file=$(cat $sha | sort -u)

	ones=$(echo "$file" | egrep " 1$" | cut -d ' ' -f 1)
	notOnes=$(echo "$file" | egrep -v " 1$" | cut -d ' ' -f 1)

	if [[ -z $notOnes ]]; then
		toDelete=$(echo "$ones" | tail -n +2)
		
		[[ -z $toDelete ]] && echo "File with inode number $ones is just 1" && continue

		while read inode; do
			find $1 -type f -inum "$inode"
		done < <(echo "$toDelete")
	elif [[ -z $ones ]]; then
		while read inode; do
			find $1 -type f -inum "$inode" | head -n 1
		done < <(echo "$notOnes")
	else
		while read inode; do
			find $1 -type f -inum "$inode"
		done < <(echo "$ones")

		while read inode; do
			find $1 -type f -inum "$inode" | head -n 1
		done < <(echo "$notOnes")
	fi
done < <(echo "$files")

rm -r $DIR
