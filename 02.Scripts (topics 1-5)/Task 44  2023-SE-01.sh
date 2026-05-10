#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 <bad_words_file> <dir>" >&2
    exit 1
fi

bad="$1"
dir="$2"

if [[ ! -f "$bad" ]]; then
    echo "Error: bad words file does not exist" >&2
    exit 1
fi

if [[ ! -d "$dir" ]]; then
    echo "Error: directory does not exist" >&2
    exit 1
fi

find "$dir" -type f -name '*.txt' | while read -r file; do

    while read -r word; do

        [[ -z "$word" ]] && continue

        stars=$(echo "$word" | sed 's/./*/g')

        sed -i "s/\<$word\>/$stars/g" "$file"

    done < "$bad"

done


############################################3
#!/bin/bash

if [[ $# -ne 2 ]]; then
    exit 1
fi

bad_words_file="$1"
dir="$2"

if [[ ! -f "$bad_words_file" ]]; then
    exit 2
fi

if [[ ! -d "$dir" ]]; then
    exit 3
fi


find "$dir" -type f -name '*.txt' | while read -r file;do
  while read -r word
  if [[ -z "$word" ]];then
    continue;
  fi
  censored=$(echo "$word" | sed 's/./*/g')
 if grep -qw "$word" "$file";then
   sed -i "s/\b$word\b/$censored/g' "$file"  #\b означава граница на дума (word boundary).
fi
  done< "$bad_words_file"
done
