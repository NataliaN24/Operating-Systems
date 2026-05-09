#!/bin/bash

if [[ $# -eq 0 ]]; then
    exit 1
fi

options=""
args=""
jar_file=""
seen_jar=0
found_file=0

while [[ $# -gt 0 ]]; do
    x="$1"
    shift

    if [[ "$x" == "-jar" ]]; then
        seen_jar=1
        continue
    fi

    if [[ "$found_file" -eq 1 ]]; then
        args="$args $x"
        continue
    fi

    if [[ "$seen_jar" -eq 0 ]]; then
        options="$options $x"
        continue
    fi

    if [[ "$x" == -* ]]; then
        options="$options $x"
    else
        jar_file="$x"
        found_file=1
    fi
done

if [[ -z "$jar_file" ]]; then
    exit 2
fi

java $options -jar "$jar_file" $args
