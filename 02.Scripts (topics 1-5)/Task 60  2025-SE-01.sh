#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 <config> <input>"
    exit 1
fi

config="$1"
input="$2"

if [[ ! -f "$config" || ! -f "$input" ]]; then
    echo "Invalid file"
    exit 1
fi

filename=$(basename "$input")
withoutExtension="${filename%.*}"

while read -r line; do
    lang=$(echo "$line" | cut -d ' ' -f1)

    #dirBase=$(echo "$line" | grep -o "'[^']*'")
    dirBase=$(echo "$line" | awk '{print $NF}' | tr -d "'")

    dir="${dirBase%\'}"
    dir="${dir#\'}"

    isVisitor=false
    isListener=false

    echo "$line" | grep -q "visitor"
    if [[ $? -eq 0 ]]; then
        isVisitor=true
    fi

    echo "$line" | grep -q "listener"
    if [[ $? -eq 0 ]]; then
        isListener=true
    fi

    opts=""

    if [[ "$isListener" == "false" ]]; then
        opts="$opts -no-listener"
    fi

    if [[ "$isVisitor" == "true" ]]; then
        opts="$opts -visitor"
    fi

    antlr4 -Dlanguage="$lang" $opts -o "$dir/$withoutExtension" "$input"

done < "$config"
####################################################################################
#/bin/bash

if [[ $# -ne 2 ]];then
  exit 1
fi

 config="${1}"
 inputFile="${2}"

while IFS= read -r line;do

  language=$(echo "$line" | cut -d ' ' -f1)
  type=$(echo "$line" | egrep -o 'recognizer|listener|visitor')
  dirbase=$(echo "$line"| awk '{print $NF}' | sed "s/'//g" )
  
  options="-no-listener"
  
  for f in $type;do
    if [[ "$f" == "listener" ]];then
      options="" 
      
    elif [[ "$f" == "vistor" ]];then
      options="$options -visitor"
    fi
  done


    subDir=$(echo "$inputFile" |  sed 's/.*\///')
    withoutExtension="${subDir%.*}"
    outputDir="$dirbase/$withoutExtension"
    
  antlr4 -Dlanguage="$language" $options -o  "$outputDir" "$inputFile"

done < "$config"
