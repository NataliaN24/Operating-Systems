#!/bin/bash

if [[ "$#" -ne 2 ]]; then
    exit 1
fi

oldDir="$1"
newDir="$2"

if [[ ! -d "$oldDir" ]]; then
    exit 1
fi

mkdir -p "$newDir"

find "$oldDir" -type f -name "*.bcf" | while read -r file; do

    relative=$(echo "$file" | sed "s|^$oldDir/||")
    output="$newDir/${relative%.bcf}.bcf2"

    mkdir -p "$(dirname "$output")"
    > "$output"

    keys=$(cut -d '=' -f1 "$file" | sort | uniq -c | tr -s ' ' | sed 's/^ //' | sort -k2)

    echo "$keys" | while read -r count key; do

        if [[ "$count" -gt 1 ]]; then
            echo "$key:" >> "$output"

            grep "^$key=" "$file" | cut -d '=' -f2- | while read -r value; do
                echo "- $value" >> "$output"
            done

        else
            value=$(grep "^$key=" "$file" | cut -d '=' -f2-)
            echo "$key: $value" >> "$output"
        fi

    done

done
#############################################################################################################################
#!/bin/bash

if [[ $# -ne 2 ]]; then
  exit 1
fi

oldDir="$1"
newDir="$2"

if [[ ! -d "$oldDir" ]]; then
  exit 2
fi

while read -r file; do
  relFile=$(echo "$file" | sed "s#^$oldDir/##")
  relDir=$(dirname "$relFile")
  baseName=$(basename "$file" .bcf)

  mkdir -p "$newDir/$relDir"
  newFile="$newDir/$relDir/$baseName.bcf2"
  : > "$newFile"

  key=$(cut -d '=' -f1 "$file" | sort | uniq -c | sort -nr)

  while read -r k; do
      howManyTimes=$(echo "$k" | sed 's/^ *//' | cut -d ' ' -f1)
      onlyKey=$(echo "$k" | sed 's/^ *//' | cut -d ' ' -f2)

      value=$(grep "^$onlyKey=" "$file" | cut -d '=' -f2-)

      if [[ "$howManyTimes" -eq 1 ]]; then
          echo "$onlyKey: $value" >> "$newFile"
      else
          echo "$onlyKey:" >> "$newFile"
          while read -r values; do
              echo "- $values" >> "$newFile"
          done <<< "$value"
      fi

  done <<< "$key"

done < <(find "$oldDir" -type f -name "*.bcf")
