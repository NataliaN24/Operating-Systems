#!/bin/bash

if [[ $# -ne 0 ]]; then
  echo "Usage: $0" >&2
  exit 1
fi

tempFile=$(mktemp)

while IFD=: read -r user homeDir;
do
  if [[ -d "${homeDir}" ]];
  then 
    file=$(find "${homeDir}" -type f -exec stat -c "%n %Y" {} \; 2>/dev/null | sort -nr | head 1)
      if [[ -n "$file" ]]; then  #not empty
      echo "${file} ${user}" >> "${tempFile}"
      fi
    else
    continue
  fi
done < <(cut -d ':' -f6 /etc/passwd)

sort -nr "${tempFile}" | head -n1
rm -f "${tempFile}
