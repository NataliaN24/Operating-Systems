#!/bin/bash

if [[ $# -lt 2 ]]; then
  echo "Error" >&2
  exit 1
fi

jar=""
jarFile=""

options="$(mktemp)"
args="$(mktemp)"
trap 'rm -f "$options" "$args"' EXIT

for arg in "$@"; do
  # 1) Ако вече имаме jarFile -> всичко след него са args
  if [[ -n "$jarFile" ]]; then
    echo "$arg" >> "$args"
    continue
  fi

  # 2) Ако започва с '-' -> опция/флаг
  if [[ "$arg" =~ ^- ]]; then
    # срещнахме -jar
    if [[ "$arg" == "-jar" ]]; then
      jar="$arg"
      continue
    fi

    # записваме като option (логиката ти е такава)
    echo "$arg" >> "$options"

    # специално правило за -D...
    if [[ "$arg" =~ ^-D ]]; then
      if [[ -n "$jar" ]]; then
        : # ок, вече е записано в options
      else
        echo "Error -Dprop=val can't be passed before -jar" >&2
        exit 1
      fi
    fi

  # 3) Не започва с '-' -> може да е jarFile (само ако вече е срещнат -jar)
  else
    if [[ -n "$jar" ]]; then
      jarFile="$arg"
      # не го пишем в args (jarFile не е аргумент за main)
    fi
  fi
done

# Проверки, че имаме -jar и jarFile
if [[ -z "$jar" ]]; then
  echo "Error: missing -jar" >&2
  exit 1
fi

if [[ -z "$jarFile" ]]; then
  echo "Error: jar file not found after -jar" >&2
  exit 1
fi

# Събираме опциите и аргументите на 1 ред (по твоята идея)
opts="$(tr '\n' ' ' < "$options" | sed 's/[[:space:]]*$//')"
arguments="$(tr '\n' ' ' < "$args" | sed 's/[[:space:]]*$//')"

# ВАЖНО: правилният стандартен синтаксис е:
# java [options] -jar jarFile [args]
# (ти си държал options отделно, args отделно -> така го изпълняваме)
java $opts -jar "$jarFile" $arguments
exit $?
