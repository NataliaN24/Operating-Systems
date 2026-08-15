#!/bin/bash

if [[ $# -ne 2 ]]; then
    exit 1
fi

dir="$1"
number="$2"

if [[ ! -d "$dir" ]]; then
    exit 2
fi

if [[ ! "$number" =~ ^[0-9]+$ ]]; then
    exit 2
fi

if (( number < 1 || number > 99 )); then
    exit 3
fi

howMuchUsed=$(df "$dir" | tr -s ' ' | sed 's/^ //; s/ $//' |
    tail -n 1 | cut -d ' ' -f5 | sed 's/%//')

if (( howMuchUsed <= number )); then
    exit 0
fi

daily="$dir/3"
weekly="$dir/2"
monthly="$dir/1"
yearly="$dir/0"

if [[ ! -d "$daily" || ! -d "$weekly" ||
      ! -d "$monthly" || ! -d "$yearly" ]]; then
    exit 2
fi

allObjects=$(mktemp)
objects=$(mktemp)
delete=$(mktemp)

find "$dir" \( -type f -o -type l \) -name '*-*.tar.xz' |
    sort -u > "$allObjects"

# Get all objects
while read -r obj; do

    if [[ -L "$obj" ]]; then
        continue
    fi

    file=$(basename "$obj")
    name=${file%.tar.xz}
    object=$(echo "$name" | cut -d '-' -f1,2)

    echo "$object"

done < "$allObjects" | sort -u > "$objects"


# Process every object
while read -r object; do

    howManyDaily=$(mktemp)
    howManyWeekly=$(mktemp)
    howManyMonthly=$(mktemp)
    howManyYearly=$(mktemp)

    find "$daily" -type f -name "$object-*.tar.xz" |
        sort -r > "$howManyDaily"

    find "$weekly" -type f -name "$object-*.tar.xz" |
        sort -r > "$howManyWeekly"

    find "$monthly" -type f -name "$object-*.tar.xz" |
        sort -r > "$howManyMonthly"

    find "$yearly" -type f -name "$object-*.tar.xz" |
        sort -r > "$howManyYearly"

    dailyCnt=$(wc -l < "$howManyDaily")
    weeklyCnt=$(wc -l < "$howManyWeekly")
    monthlyCnt=$(wc -l < "$howManyMonthly")
    yearlyCnt=$(wc -l < "$howManyYearly")

    # Object is not valid -> do not delete any full backup
    if (( dailyCnt < 4 || weeklyCnt < 3 ||
          monthlyCnt < 2 || yearlyCnt < 1 )); then

        rm -f "$howManyDaily" "$howManyWeekly"
        rm -f "$howManyMonthly" "$howManyYearly"

        continue
    fi

    # Keep newest 4 daily backups
    tail -n +5 "$howManyDaily" >> "$delete"

    # Keep newest 3 weekly backups
    tail -n +4 "$howManyWeekly" >> "$delete"

    # Keep newest 2 monthly backups
    tail -n +3 "$howManyMonthly" >> "$delete"

    # Keep newest 1 yearly backup
    tail -n +2 "$howManyYearly" >> "$delete"

    rm -f "$howManyDaily" "$howManyWeekly"
    rm -f "$howManyMonthly" "$howManyYearly"

done < "$objects"


# Delete in order of importance:
#
# yearly (0) -> monthly (1) -> weekly (2) -> daily (3)
# and oldest first inside each class.
#
# We determine the class from the directory name.

for class in 0 1 2 3; do

    if (( class == 0 )); then
        find "$yearly" -type f -name '*-*.tar.xz' |
            sort >> /tmp/delete_all
    fi

    if (( class == 1 )); then
        find "$monthly" -type f -name '*-*.tar.xz' |
            sort >> /tmp/delete_all
    fi

    if (( class == 2 )); then
        find "$weekly" -type f -name '*-*.tar.xz' |
            sort >> /tmp/delete_all
    fi

    if (( class == 3 )); then
        find "$daily" -type f -name '*-*.tar.xz' |
            sort >> /tmp/delete_all
    fi

done

# Instead of /tmp/delete_all, create the ordered list directly
# from the files that are actually allowed to be deleted.
rm -f /tmp/delete_all

find "$yearly" -type f -name '*-*.tar.xz' |
    sort >> "$delete"

find "$monthly" -type f -name '*-*.tar.xz' |
    sort >> "$delete"

find "$weekly" -type f -name '*-*.tar.xz' |
    sort >> "$delete"

find "$daily" -type f -name '*-*.tar.xz' |
    sort >> "$delete"


while read -r file; do

    howMuchUsedActual=$(df "$dir" | tr -s ' ' | sed 's/^ //; s/ $//' |
        tail -n 1 | cut -d ' ' -f5 | sed 's/%//')

    if (( howMuchUsedActual <= number )); then
        break
    fi

    if [[ -f "$file" && ! -L "$file" ]]; then
        rm -f "$file"
    fi

done < "$delete"


# Remove broken symlinks
find "$dir" -type l ! -e -delete

rm -f "$allObjects" "$objects" "$delete"



///////////////////////////////////////////////
#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 <fubar_dir> <percent>" >&2
    exit 1
fi

fubar="$1"
max="$2"

if [[ ! -d "$fubar" ]]; then
    echo "Invalid directory" >&2
    exit 1
fi

if [[ "$max" -lt 1 || "$max" -gt 99 ]]; then
    echo "Percent must be in [1,99]" >&2
    exit 1
fi

for d in 0 1 2 3; do
    if [[ ! -d "$fubar/$d" ]]; then
        echo "Missing directory $d" >&2
        exit 1
    fi
done

# трием счупени symlink-ове
find "$fubar" -type l ! -exec test -e {} \; -delete

tmp=$(mktemp)

for class in 0 1 2 3; do

    find "$fubar/$class" -type f -name '*.tar.xz' | while read -r file; do

        name=$(basename "$file")

        host=$(echo "$name" | cut -d '-' -f1)
        area=$(echo "$name" | cut -d '-' -f2)
        date=$(echo "$name" | cut -d '-' -f3 | cut -d '.' -f1)

        object="$host-$area"

        echo "$class $object $date $file" >> "$tmp"

    done

done

candidates=$(mktemp)

for class in 0 1 2 3; do

    if [[ "$class" -eq 0 ]]; then
        min=1
    elif [[ "$class" -eq 1 ]]; then
        min=2
    elif [[ "$class" -eq 2 ]]; then
        min=3
    else
        min=4
    fi

    grep "^$class " "$tmp" | cut -d ' ' -f2 | sort | uniq | while read -r object; do

        count=$(grep "^$class $object " "$tmp" | wc -l)

        if [[ "$count" -gt "$min" ]]; then

            extra=$((count - min))

            grep "^$class $object " "$tmp" |
            sort -k3,3 |
            head -n "$extra" |
            while read -r c obj date path; do

                echo "$c $date $path" >> "$candidates"

            done
        fi

    done

done

sort -k1,1n -k2,2n "$candidates" | while read -r class date file; do

    used=$(df "$fubar" | tail -n 1 | tr -s ' ' | cut -d ' ' -f5)
    used="${used%\%}"

    if [[ "$used" -le "$max" ]]; then
        break
    fi

    rm -- "$file"

done

# пак махаме счупени symlink-ове
find "$fubar" -type l ! -exec test -e {} \; -delete

rm "$tmp"
rm "$candidates"


####################################################################################################
if [[ ${#} -ne 2 ]] ; then
  echo "Expected 2 arguments"
  exit 1
fi
DIR=${1}

if [[ ! -d ${DIR} ]] ; then
  echo "First argument should be a directory"
  exit 1
fi

NUMBER=${2}

if [[ ${NUMBER} -lt 1 ]] || [[ ${NUMBER} -gt 99]];then
echo "Number should be in interval [1,99]"
  exit 1
fi

objects=$(find "${DIR}" -mindepth 1 -type f -printf '%f\n' |  awk -F '{print $1 "-" $2}' | sort -u)
for obj in $(echo "${objects}");do
  result=$(find "${DIR}" -type f -name "${object}*")
  yearBackup=$(echo "${result}" | awk -F '/' '{if ($(NF -1)=="0") {print $0}')
  monthBackup=$(echo "${result}" | awk -F '/' '{if ($(NF-1)=="1") {print $0}')
  weeklyBackup=$(echo "${result}" | awk -F '/' '{if ($(NF-1)=="2") {print $0}')
  dailyBackup=$(echo "${result}" | awk -F '/' '{if ($(NF-1)=="3") {print $0}')

  yearCount=$(echo "${yearBackup}" | wc -l)
  monthCount=$(echo "${monthBackup}" | wc -l)
  weeklyBackup=$(echo "${weeklyBackup}" | wc -l)
  dailyBackup=$(echo "${dailyBackup}" | wc -l)

  if [[ ${yearCount} -gt 1 ]];then
  toDelete =$(( yearCount -1 ))
  for file in $(find "${DIR}/0" -mindepth 1 -name "${object}*" -type f | sort | head -n ${toDelete});do
     rm ${file}
        usage=$(df ${DIR} | tail -n1 |awk '{print $5}' | sed -E 's/%$//')
            if [[ ${usage} -le ${NUMBER} ]] ; then
               exit 0
             fi
   done
  elif [[ ${monthCount} -gt 2]];then
    toDelete=$((monthCount -2))
    for file in $(find "${DIR}/1" -mindepth 1 -name "${object}*" -type f | sort | head -n ${toDelete});do
       rm ${file}
      usage=$(df ${DIR} | tail -n1 | awk '{print $5}' | sed -E 's/%$//')
                        if [[ ${usage} - le ${NUMBER} ]] ; then
                                exit 0
                        fi
                done
        elif [[ ${weeklyCount} -gt 3 ]] ; then
                toDelete=$(( weeklyCount - 3 ))
                for file in $(find "${DIR}/2" -mindepth 1 -name "${object}*" -type f | sort | head -n ${toDelete}) ; do
                        rm ${file}
                        usage=$(df ${DIR} | tail -n1 |awk '{print $5}' | sed -E 's/%$//')
                        if [[ ${usage} -le ${NUMBER} ]] ; then
                                exit 0
                        fi
                done
        elif [[ ${dailyCount} -gt 4 ]] ; then
                toDelete=$(( dailyCount - 4 )) ; then
                for file in $(find "${DIR}/3" -mindepth 1 -name "${object}*" -type f | sort | head -n ${toDelete}) ; do
                        rm ${file}
                        usage=$(df ${DIR} | tail -n1 | awk '{print $5}' | sed -E 's/%$//')
                        if [[ ${usage} - le ${NUMBER} ]] ; then
                                exit 0
                        fi
                done
        fi

  find "${DIR}" -mindepth 1 -type l -exec test ! -e {} \; -print | xargs -I{} rm {}
  
