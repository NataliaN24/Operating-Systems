#!/bin/bash

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <logfile>" >&2
  exit 1
fi

logfile="$1"
if [[ ! -f "$logfile" || ! -r "$logfile" ]]; then
  echo "Error: cannot read file: $logfile" >&2
  exit 1
fi

#35.223.122.181 dir.bg - [03/Apr/2020:17:25:06 -0500] GET / HTTP/1.1 302 0 "-" "Zend_Http_Client"
#94.228.82.170 del.bg - [03/Apr/2020:17:25:06 -0500] POST /auth HTTP/2.0 400 153 "foo bar" "<UA>"

top_sites=$(
  cut -d ' ' -f2 "$logfile" \
    | sort \
    | uniq -c \
    | sort -nr \
    | head -3 \
    | awk '{print $2}'
)

for site in $top_Sites
do

 # всички заявки за този сайт
allQueries=$(grep -E "^[^ ]+$site"  "$logfile")

# HTTP/2.0 заявки
http2=$(echo "$allQueries" | grep -c "HTTP/2.0")

# всички заявки
totalQueriesNumber=$(echo "$allQueries" | wc -l)

nonHttp2=$((totalQueriesNumber-http2))

echo "$site HTTP/2.0: $http2 non-HTTP/2.0: $nonHttp2"

clients=$(echo "$allQueries" | awk '$8 >302 {print $1}; | sort | uniq - | sort -nr | head -n 5)
echo "$clients"

done
