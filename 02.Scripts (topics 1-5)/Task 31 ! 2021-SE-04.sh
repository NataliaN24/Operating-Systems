#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 hours" >&2
    exit 1
fi

hours="$1"

if ! [[ "$hours" =~ ^[0-9]+$ ]]; then
    echo "Error: hours must be a number" >&2
    exit 1
fi

if [[ "$hours" -lt 2 ]]; then
    echo "Error: minimum is 2 hours" >&2
    exit 1
fi

user=$(whoami)

if [[ "$user" != "oracle" && "$user" != "grid" ]]; then
    echo "Error: script must be run as oracle or grid" >&2
    exit 1
fi

if [[ -z "$ORACLE_HOME" ]]; then
    echo "Error: ORACLE_HOME is not set" >&2
    exit 1
fi

adrci="$ORACLE_HOME/bin/adrci"

if [[ ! -x "$adrci" ]]; then
    echo "Error: $adrci is not executable" >&2
    exit 1
fi

diag_dest="/u01/app/$user"
minutes=$((hours * 60))

homes=$(
    "$adrci" exec="SET BASE $diag_dest; SHOW HOMES" |
    grep '^diag/' |
    grep -E '^diag/(crs|tnslsnr|kfod|asm|rdbms)/'
)

if [[ -z "$homes" ]]; then
    exit 0
fi

echo "$homes" | while read home; do
    "$adrci" exec="SET BASE $diag_dest; SET HOMEPATH $home; PURGE -AGE $minutes"
done
