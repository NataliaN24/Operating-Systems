#!/bin/bash

if [[ -z "$SVC_DIR" ]]; then
    exit 1
fi

if [[ ! -d "$SVC_DIR" ]]; then
    exit 2
fi

if [[ $# -eq 2 ]]; then
    firstArg="$1"
    servName="$2"
elif [[ $# -eq 1 ]]; then
    firstArg="$1"
else
    exit 3
fi

if [[ "$firstArg" == "start" || "$firstArg" == "stop" ]]; then
    found=0

    while read -r file; do
        name=$(grep "^name:" "$file" | cut -d ' ' -f2)
        pidFile=$(grep "^pidfile:" "$file" | cut -d ' ' -f2)
        outFile=$(grep "^outfile:" "$file" | cut -d ' ' -f2)
        comm=$(grep "^comm:" "$file" | cut -d ' ' -f2-)

        if [[ "$name" == "$servName" ]]; then
            found=1

            if [[ "$firstArg" == "start" ]]; then
                if [[ -f "$pidFile" ]]; then
                    pid=$(cat "$pidFile")
                    if ps -p "$pid" > /dev/null 2>&1; then
                        exit 0
                    fi
                fi

                bash -c "$comm" > "$outFile" 2>&1 &
                echo "$!" > "$pidFile"
                exit 0

            elif [[ "$firstArg" == "stop" ]]; then
                if [[ -f "$pidFile" ]]; then
                    pid=$(cat "$pidFile")
                    if ps -p "$pid" > /dev/null 2>&1; then
                        kill -TERM "$pid"
                    fi
                fi
                exit 0
            fi
        fi
    done < <(find "$SVC_DIR" -maxdepth 1 -type f)

    if [[ "$found" -eq 0 ]]; then
        exit 4
    fi

elif [[ "$firstArg" == "running" ]]; then
    while read -r file; do
        name=$(grep "^name:" "$file" | cut -d ' ' -f2)
        pidFile=$(grep "^pidfile:" "$file" | cut -d ' ' -f2)

        if [[ -f "$pidFile" ]]; then
            pid=$(cat "$pidFile")
            if ps -p "$pid" > /dev/null 2>&1; then
                echo "$name"
            fi
        fi
    done < <(find "$SVC_DIR" -maxdepth 1 -type f) | sort

elif [[ "$firstArg" == "cleanup" ]]; then
    while read -r file; do
        pidFile=$(grep "^pidfile:" "$file" | cut -d ' ' -f2)

        if [[ -f "$pidFile" ]]; then
            pid=$(cat "$pidFile")
            if ! ps -p "$pid" > /dev/null 2>&1; then
                rm -f "$pidFile"
            fi
        fi
    done < <(find "$SVC_DIR" -maxdepth 1 -type f)

else
    exit 5
fi
