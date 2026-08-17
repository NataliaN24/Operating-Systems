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

while read -r line; do

    # Премахваме коментара
    line=$(echo "$line" | sed 's/#.*//')

    if [[ -z "$line" ]]; then
        continue
    fi

    # CMD
    if [[ "$line" == CMD* ]]; then
        cmd=$(echo "$line" | cut -d' ' -f2-)

    # ARGS
    elif [[ "$line" == ARGS* ]]; then
        args=$(echo "$line" | sed 's/^ARGS //' | sed 's/^\[//' | sed 's/\]$//' | sed 's/", "/\n/g' | tr -d '"')

    # ENV
    elif [[ "$line" == ENV* ]]; then
        envs=$(echo "$line" | sed 's/^ENV //' | sed 's/^{//' | sed 's/}$//' | sed 's/", "/\n/g' | tr -d '"')

    # WORKDIR
    elif [[ "$line" == WORKDIR* ]]; then
        workdir=$(echo "$line" | cut -d' ' -f2-)
    fi

done < "$file"

if [[ -n "$workdir" ]]; then
    cd "$workdir" || exit 3
fi

echo "$args" > args.tmp
echo "$envs" > envs.tmp

command="$cmd"

while read -r arg; do
    command="$command \"$arg\""
done < args.tmp

while read -r env; do
    command="$env $command"
done < envs.tmp

eval "$command"

rm args.tmp envs.tmp
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
