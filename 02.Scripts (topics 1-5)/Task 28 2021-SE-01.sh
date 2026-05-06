#!/bin/bash

user=$(whoami)

if [[ "$user" != "oracle" && "$user" != "grid" ]]; then
    echo "Must be oracle or grid"
    exit 1
fi

if [[ -z "$ORACLE_HOME" ]]; then
    echo "ORACLE_HOME is not set"
    exit 1
fi

adrci=$(find "$ORACLE_HOME" -type f -name "adrci")

if [[ -z "$adrci" ]]; then
    echo "adrci not found"
    exit 1
fi

diag_dest="/u01/app/$user"

"$adrci" exec="show homes" | tail -n +2 | while read -r home; do

    if [[ "$home" == "No ADR homes are set" ]]; then
        exit 0
    fi

    full_path="$diag_dest/$home"

    size=$(du -sm "$full_path" 2>/dev/null | cut -f1)

    echo "$size $full_path"

done
