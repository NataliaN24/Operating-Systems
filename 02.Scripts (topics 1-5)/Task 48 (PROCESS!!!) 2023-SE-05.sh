
#!/bin/bash

limit=65536
total_checks=0

tmp=$(mktemp)

while true; do
    total_checks=$((total_checks + 1))

    current=$(mktemp)

    ps -e -o comm=,rss= | awk -v limit="$limit" '
    {
        sum[$1] += $2
    }
    END {
        found = 0
        for (cmd in sum) {
            if (sum[cmd] > limit) {
                print cmd
                found = 1
            }
        }
        if (found == 0) {
            exit 1
        }
    }' > "$current"

    if [[ ! -s "$current" ]]; then
        rm -f "$current"
        break
    fi

    while read -r cmd; do
        echo "$cmd"
    done < "$current" >> "$tmp"

    rm -f "$current"
    sleep 1
done

sort "$tmp" | uniq -c | awk -v total="$total_checks" '
{
    if ($1 * 2 >= total) {
        print $2
    }
}
'

rm -f "$tmp"
