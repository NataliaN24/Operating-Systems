

#!/bin/bash

if [[ $# -ne 2 ]]; then
    exit 1
fi

domain="$1"
file="$2"

if [[ ! -f "$file" ]]; then
    exit 2
fi

teams=$(mktemp)

cat "$file" | tr -s ' ' | cut -d ' ' -f3 | sort | uniq > "$teams"

while read -r artist; do
    echo "; team $artist"

    zones=$(grep -E " $artist$" "$file" | tr -s ' ' | cut -d ' ' -f2 | sort | uniq)
    servers=$(grep -E " $artist$" "$file" | tr -s ' ' | cut -d ' ' -f1 | sort | uniq)

    for z in $zones; do
        for s in $servers; do
            echo "$z IN NS $s.$domain."
        done
    done

done < "$teams"

rm -f "$teams"

##########################################################################################################





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
