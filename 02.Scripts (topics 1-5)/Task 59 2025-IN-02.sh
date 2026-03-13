
#!/bin/bash

if [[ $# -ne 2 ]]; then
    exit 1
fi

domain="$1"
file="$2"

if [[ ! -f "$file" ]]; then
    exit 2
fi

artists=$(cat "$file" |  cut -d ' ' -f3 | sort -u)

for art in $artists;do
  echo " ; team $art"
  composer=$(cat "$file" | grep "$art" | cut -d ' ' -f2 )
  server=$(cat "$file" | grep "$art" | cut -d ' ' -f1)
  
   for compo in $composer;do
    for ser in $server;do
        echo "$compo IN NS $ser.$domain."
      done
    done
done
