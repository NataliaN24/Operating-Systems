#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

if [[ ! -d "$1" ]]; then
    exit 2
fi

dir="$1"
groupId=$(stat -c '%g' "$dir")
user=$(whoami)

if [[ "$user" == "root" ]]; then

    find "$dir" | while read -r file; do

        chgrp "$groupId" "$file"

        if [[ -d "$file" ]]; then
            chmod 2770 "$file"  #we have 2 in the beginning because of setgid -> new file will inherit that group 
        elif [[ -f "$file" ]]; then
            chmod ug+rw,o-rwx "$file"
        fi

    done

    exit 0
fi

if ! id -G "$user" | grep -qw "$groupId"; then
    exit 3
fi

umask 0007
#####################################################################



#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <shared-dir>" >&2
    exit 1
fi

dir="$1"

if [[ ! -d "$dir" ]]; then
    echo "Error: not a directory" >&2
    exit 1
fi

group=$(stat -c '%G' "$dir")

if [[ "$(id -u)" -eq 0 ]]; then
    chgrp -R "$group" "$dir"

    find "$dir" -type d -exec chmod u=rwx,g=rwx,o=,g+s {} \;

    find "$dir" -type f -exec chmod u+rw,g+rw,o-rwx {} \;

    exit 0
fi

if ! id -nG | grep -qw "$group"; then
    echo "Error: user is not member of group $group" >&2
    exit 1
fi

umask 007
echo "umask set to 007"

############################################################################################

#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

dir="$1"

if [[ ! -d "$dir" ]]; then
    exit 2
fi

user=$(whoami)
dirGroup=$(stat -c "%G" "$dir")

if [[ "$user" == "root" ]]; then
    while read -r file; do
     chown :"$dirGroup" "$file"
        if [[ -d "$file" ]]; then
            chmod 770 "$file"
        elif [[ -f "$file" ]]; then
            chmod u+rw,g+rw,o-rwx "$file"
        fi
    done < <(find "$dir")

    exit 0
fi

userGroups=$(id -nG "$user")

found=$(echo "$userGroups" | tr ' ' '\n' | grep -x "$dirGroup")

if [[ -z "$found" ]]; then
    exit 3
fi

umask 007
