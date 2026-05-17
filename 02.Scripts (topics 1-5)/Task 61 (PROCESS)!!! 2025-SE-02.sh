#!/bin/bash

if [[ -z "$SVC_DIR" || ! -d "$SVC_DIR" ]]; then
    exit 1
fi

[[ "$#" -lt 1 ]] && exit 1

command="$1"

if [[ "$command" == "start" ]]; then

    [[ "$#" -ne 2 ]] && exit 1

    service="$2"
    service_file=""

    for file in "$SVC_DIR"/*; do
        [[ -f "$file" ]] || continue

        name=$(grep "^name:" "$file" | cut -d ' ' -f2)

        if [[ "$name" == "$service" ]]; then
            service_file="$file"
            break
        fi
    done

    [[ -z "$service_file" ]] && exit 1

    pidfile=$(grep "^pidfile:" "$service_file" | cut -d ' ' -f2)
    outfile=$(grep "^outfile:" "$service_file" | cut -d ' ' -f2)
    comm=$(grep "^comm:" "$service_file" | cut -d ' ' -f2-)

    running=0

    if [[ -f "$pidfile" ]]; then
        pid=$(cat "$pidfile")

        if ps -p "$pid" > /dev/null 2>&1; then
            running=1
        fi
    fi

    if [[ "$running" -eq 0 ]]; then
        bash -c "$comm" > "$outfile" 2>&1 &
        echo "$!" > "$pidfile"
    fi

elif [[ "$command" == "stop" ]]; then

    [[ "$#" -ne 2 ]] && exit 1

    service="$2"
    service_file=""

    for file in "$SVC_DIR"/*; do
        [[ -f "$file" ]] || continue

        name=$(grep "^name:" "$file" | cut -d ' ' -f2)

        if [[ "$name" == "$service" ]]; then
            service_file="$file"
            break
        fi
    done

    [[ -z "$service_file" ]] && exit 1

    pidfile=$(grep "^pidfile:" "$service_file" | cut -d ' ' -f2)

    if [[ -f "$pidfile" ]]; then
        pid=$(cat "$pidfile")
        kill -TERM "$pid" 2>/dev/null
    fi

elif [[ "$command" == "running" ]]; then

    [[ "$#" -ne 1 ]] && exit 1

    for file in "$SVC_DIR"/*; do
        [[ -f "$file" ]] || continue

        name=$(grep "^name:" "$file" | cut -d ' ' -f2)
        pidfile=$(grep "^pidfile:" "$file" | cut -d ' ' -f2)

        if [[ -f "$pidfile" ]]; then
            pid=$(cat "$pidfile")

            if ps -p "$pid" > /dev/null 2>&1; then
                echo "$name"
            fi
        fi
    done | sort

elif [[ "$command" == "cleanup" ]]; then

    [[ "$#" -ne 1 ]] && exit 1

    for file in "$SVC_DIR"/*; do
        [[ -f "$file" ]] || continue

        pidfile=$(grep "^pidfile:" "$file" | cut -d ' ' -f2)

        if [[ -f "$pidfile" ]]; then
            pid=$(cat "$pidfile")

            if ! ps -p "$pid" > /dev/null 2>&1; then
                rm -- "$pidfile"
            fi
        fi
    done

else
    exit 1
fi
###############################################################################################################
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
