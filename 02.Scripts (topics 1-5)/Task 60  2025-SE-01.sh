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
