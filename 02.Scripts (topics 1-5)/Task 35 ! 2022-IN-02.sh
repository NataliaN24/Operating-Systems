#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

days="$1"

user=$(whoami)

if [[ "$user" != "grid" && "$user" != "oracle" ]]; then
    exit 2
fi

if [[ -z "$ORACLE_BASE" || -z "$ORACLE_HOME" || -z "$ORACLE_SID" ]]; then
    exit 3
fi

sqlPlus="$ORACLE_HOME/bin/sqlplus"

if [[ ! -x "$sqlPlus" ]]; then
    exit 4
fi

role="SYSDBA"

if [[ "$user" == "grid" ]]; then
    role="SYSASM"
fi

result=$("$sqlPlus" -SL "/ as $role" @foo.sql)

if [[ $? -ne 0 ]]; then
    exit 5
fi

diagnostic_dest=$(echo "$result" | tail -n +4 | head -n 1 | tr -d ' ')

diag_base="$ORACLE_BASE"

if [[ -n "$diagnostic_dest" ]]; then
    diag_base="$diagnostic_dest"
fi

diag_dir="$diag_base/diag"

if [[ ! -d "$diag_dir" ]]; then
    exit 6
fi

machineName=$(hostname -s)

sum_size() {
    total=0

    while read -r file; do
        size=$(stat -c '%s' "$file")
        total=$((total + size))
    done

    echo $((total / 1024))
}

if [[ "$user" == "oracle" ]]; then

    dirrdbms="$diag_dir/rdbms"

    if [[ ! -d "$dirrdbms" ]]; then
        exit 7
    fi

    rdbms=$(
        find "$dirrdbms" \
            -mindepth 3 -maxdepth 3 \
            -type f \
            -mtime +"$days" \
            \( -name '*_[0-9]*.trc' -o -name '*_[0-9]*.trm' \) |
        sum_size
    )

    echo "rdbms: $rdbms"

elif [[ "$user" == "grid" ]]; then

    dirCRS="$diag_dir/crs/$machineName/crs/trace"

    if [[ ! -d "$dirCRS" ]]; then
        exit 8
    fi

    crs=$(
        find "$dirCRS" \
            -type f \
            -mtime +"$days" \
            \( -name '*_[0-9]*.trc' -o -name '*_[0-9]*.trm' \) |
        sum_size
    )

    tnslsnr=$(
        find "$diag_dir/tnslsnr/$machineName" \
            -type f \
            -mtime +"$days" \
            \( \
                \( -path '*/alert/*' -name '*_[0-9]*.xml' \) -o \
                \( -path '*/trace/*' -name '*_[0-9]*.log' \) \
            \) |
        sum_size
    )

    echo "crs: $crs"
    echo "tnslsnr: $tnslsnr"
fi
