#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

file="$1"

if [[ ! -f "$file" ]]; then
    exit 2
fi

cmd=""
args=""
envs=""
workdir=""

while read -r line; do

    line=$(echo "$line" | sed 's/#.*//')

    if [[ -z "$line" ]]; then
        continue
    fi

    if [[ "$line" == CMD* ]]; then
        cmd=$(echo "$line" | cut -d' ' -f2-)

    elif [[ "$line" == ARGS* ]]; then
        args=$(echo "$line" | sed 's/^ARGS //' | sed 's/^\[//' | sed 's/\]$//' | tr ',' '\n' | tr -d '"')

    elif [[ "$line" == ENV* ]]; then
        envs=$(echo "$line" | sed 's/^ENV //' | sed 's/^{//' | sed 's/}$//' | tr ',' '\n' | tr -d '"')

    elif [[ "$line" == WORKDIR* ]]; then
        workdir=$(echo "$line" | cut -d' ' -f2-)
    fi

done < "$file"

if [[ -n "$workdir" ]]; then
    cd "$workdir" || exit 3
fi

command="$cmd"

while read -r arg; do
    arg=$(echo "$arg" | sed 's/^ *//')
    command="$command \"$arg\""
done <<< "$args"

while read -r env; do
    env=$(echo "$env" | sed 's/^ *//')
    env=$(echo "$env" | sed 's/:/=/')
    command="$env $command"
done <<< "$envs"

eval "$command"
////////////////////////////////////////////////////////////////////
#!/bin/bash

if [[ $# -ne 1 ]]; then
    exit 1
fi

file="$1"

if [[ ! -f "$file" ]]; then
    exit 2
fi

cmd=""
workdir=""
args=()
envs=()

while read -r line; do
    line=$(echo "$line" | sed 's/#.*//')
    line=$(echo "$line" | sed 's/^[[:space:]]*//; s/[[:space:]]*$//')

    if echo "$line" | grep -qE "^CMD[[:space:]]+"; then
        cmd=$(echo "$line" | sed -E 's/^CMD[[:space:]]+//')

    elif echo "$line" | grep -qE "^ARGS[[:space:]]+"; then
        raw=$(echo "$line" | sed -E 's/^ARGS[[:space:]]+//')
        raw=$(echo "$raw" | sed 's/^\[//; s/\]$//')

        args=()

        while read -r arg; do
            arg=$(echo "$arg" | sed 's/^[[:space:]]*"//; s/"[[:space:]]*$//')
            args+=("$arg")
        done < <(echo "$raw" | tr ',' '\n')

    elif echo "$line" | grep -qE "^ENV[[:space:]]+"; then
        raw=$(echo "$line" | sed -E 's/^ENV[[:space:]]+//')
        raw=$(echo "$raw" | sed 's/^{//; s/}$//')

        envs=()

        while read -r pair; do
            key=$(echo "$pair" | cut -d ':' -f1 | tr -d '"' | sed 's/^[[:space:]]*//; s/[[:space:]]*$//')
            val=$(echo "$pair" | cut -d ':' -f2 | tr -d '"' | sed 's/^[[:space:]]*//; s/[[:space:]]*$//')

            envs+=("$key=$val")
        done < <(echo "$raw" | tr ',' '\n')

    elif echo "$line" | grep -qE "^WORKDIR[[:space:]]+"; then
        workdir=$(echo "$line" | sed -E 's/^WORKDIR[[:space:]]+//')
    fi

done < "$file"

if [[ -n "$workdir" ]]; then
    cd "$workdir" || exit 3
fi

if [[ -z "$cmd" ]]; then
    exit 4
fi

env "${envs[@]}" "$cmd" "${args[@]}"
