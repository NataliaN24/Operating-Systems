#!/bin/bash

sums=$(mktemp)
counts=$(mktemp)
total=0

while true; do

    # Изчистваме sums за новото поглеждане
    > "$sums"

    # Вземаме comm и rss на всички процеси
    ps -eo comm,rss | tail -n +2 | while read -r comm rss; do

        # Проверяваме дали вече сме срещали тази команда
        old=$(grep "^$comm " "$sums" | cut -d ' ' -f2)

        if [[ -z "$old" ]]; then

            # Ако я няма, записваме нейния RSS
            echo "$comm $rss" >> "$sums"

        else

            # Ако я има, събираме стария и новия RSS
            new=$((old + rss))

            # Заменяме стария RSS с новия
            sed -i "s/^$comm $old$/$comm $new/" "$sums"
        fi

    done

    # Направили сме още едно поглеждане
    total=$((total + 1))

    found=0

    # Проверяваме кои команди са над 65536
    while read -r comm rss; do

        if [[ "$rss" -gt 65536 ]]; then

            found=1

            # Колко пъти досега тази команда е била над 65536
            old=$(grep "^$comm " "$counts" | cut -d ' ' -f2)

            if [[ -z "$old" ]]; then

                # Първи път
                echo "$comm 1" >> "$counts"

            else

                # Увеличаваме брояча
                new=$((old + 1))

                sed -i "s/^$comm $old$/$comm $new/" "$counts"
            fi

        fi

    done < "$sums"

    # Ако няма нито една команда над 65536, спираме
    if [[ "$found" -eq 0 ]]; then
        break
    fi

    sleep 1

done


# Извеждаме командите, които са били
# над 65536 в поне половината поглеждания

while read -r comm count; do

    if [[ $((count * 2)) -ge "$total" ]]; then
        echo "$comm"
    fi

done < "$counts"


rm "$sums" "$counts"
//////////////////////////////////////////////////////////
#!/bin/bash

limit=65536
totalChecks=0

log=$(mktemp)

while true; do
    all=$(mktemp)
    commands=$(mktemp)
    current=$(mktemp)

    ps -eo comm=,rss= | tr -s ' ' | sed 's/^ //' > "$all"
    ps -eo comm= | tr -s ' ' | sed 's/^ //' | sort -u > "$commands"

    found=0
    totalChecks=$((totalChecks + 1))

    while read -r command; do
        sum=0

        while read -r comm rss; do
            if [[ "$command" == "$comm" ]]; then
                sum=$((sum + rss))
            fi
        done < "$all"

        echo "$totalChecks $command $sum" >> "$log"

        if [[ "$sum" -gt "$limit" ]]; then
            echo "$command" >> "$current"
            found=1
        fi

    done < "$commands"

    rm "$all" "$commands" "$current"

    if [[ "$found" -eq 0 ]]; then
        break
    fi

    sleep 1
done

validChecks=$((totalChecks - 1))

cut -d ' ' -f2 "$log" | sort -u | while read -r command; do
    times=$(grep "^[0-9]\+ $command " "$log" | while read -r check cmd rss; do
        if [[ "$rss" -gt "$limit" && "$check" -le "$validChecks" ]]; then
            echo 1
        fi
    done | wc -l)

    if [[ $((times * 2)) -ge "$validChecks" ]]; then
        echo "$command"
    fi
done

rm "$log"
########################################################################################################################################
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
