#!/bin/bash

if[[ $# -ne 1]];then
  echo "only 1 argument";
  exit 1
fi

element="$1"
if[[ !element -ne [1-9]]]
