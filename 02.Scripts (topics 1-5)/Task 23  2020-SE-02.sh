#!/bin/bash

# check number of arguments
if [[ $# -ne 1 ]]; then
    echo "Usage: $0 logfile"
    exit 1
fi

file="$1"

top3=$(mktemp)

# намираме трите сайта с най-много заявки
cut -d ' ' -f2 "$file" | sort | uniq -c | sort -nr | head -n 3 | cut -d ' ' -f8 > "$top3"


while read -r site; do

    http2=$(grep " $site " "$file" | grep " HTTP/2.0 " | wc -l)

    nonhttp2=$(grep " $site " "$file" | grep -v " HTTP/2.0 " | wc -l)

    echo "$site HTTP/2.0: $http2 non-HTTP/2.0: $nonhttp2"


    clients=$(mktemp)


    grep " $site " "$file" | while read -r line; do

        code=$(echo "$line" | cut -d ' ' -f8)

        if [[ "$code" -gt 302 ]]; then

            client=$(echo "$line" | cut -d ' ' -f1)

            echo "$client" >> "$clients"

        fi

    done


    sort "$clients" | uniq -c | sort -nr | head -n 5


    rm "$clients"

done < "$top3"


rm "$top3"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 logfile"
    exit 1
fi

file="$1"

if [[ ! -f "$file" ]]; then
    echo "File does not exist"
    exit 1
fi

tmp=$(mktemp)

# top 3 сайта с най-много заявки
cut -d ' ' -f2 "$file" |
sort |
uniq -c |
sort -nr |
head -n 3 |
awk '{print $2}' > "$tmp"

while read -r site; do

    http2=$(grep " $site " "$file" | grep "HTTP/2.0" | wc -l)

    non_http2=$(grep " $site " "$file" | grep -v "HTTP/2.0" | wc -l)

    echo "$site HTTP/2.0: $http2 non-HTTP/2.0: $non_http2"

    grep " $site " "$file" |
    awk '$9 > 302 { print $1 }' |
    sort |
    uniq -c |
    sort -nr |
    head -n 5

done < "$tmp"

rm -f "$tmp"
