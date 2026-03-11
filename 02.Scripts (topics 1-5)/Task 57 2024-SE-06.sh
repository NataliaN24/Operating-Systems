#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <spec-file>" >&2
    exit 1
fi

if [[ ! -f "$1" ]]; then
  exit 2
fi
