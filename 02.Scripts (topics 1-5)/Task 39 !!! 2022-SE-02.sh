if [[ ${#} -ne 2 ]] ; then
  echo "Expected 2 arguments"
  exit 1
fi
DIR=${1}

if [[ ! -d ${DIR} ]] ; then
  echo "First argument should be a directory"
  exit 1
fi

NUMBER=${2}

if [[ ${NUMBER} -lt 1 ]] || [[ ${NUMBER} -gt 99]];then
echo "Number should be in interval [1,99]"
  exit 1
fi

objects=$(find "${DIR}" -mindepth 1 -type f -printf '%f\n' |  awk -F '{print $1 "-" $2}' | sort -u)
for obj in $(echo "${objects}");do
  result=$(find "${DIR}" -type f -name "${object}*")
  yearBackup=$(echo "${result}" | awk -F '/' '{if ($(NF -1)=="0") {print $0}')
  monthBackup=$(echo "${result}" | awk -F '/' '{if ($(NF-1)=="1") {print $0}')
  weeklyBackup=$(echo "${result}" | awk -F '/' '{if ($(NF-1)=="2") {print $0}')
  dailyBackup=$(echo "${result}" | awk -F '/' '{if ($(NF-1)=="3") {print $0}')

  yearCount=$(echo "${yearBackup}" | wc -l)
  monthCount=$(echo "${monthBackup}" | wc -l)
  weeklyBackup=$(echo "${weeklyBackup}" | wc -l)
  dailyBackup=$(echo "${dailyBackup}" | wc -l)

  if [[ ${yearCount} -gt 1 ]];then
  toDelete =$(( yearCount -1 ))
  for file in $(find "${DIR}/0" -mindepth 1 -name "${object}*" -type f | sort | head -n ${toDelete});do
     rm ${file}
        usage=$(df ${DIR} | tail -n1 |awk '{print $5}' | sed -E 's/%$//')
            if [[ ${usage} -le ${NUMBER} ]] ; then
               exit 0
             fi
   done
  elif [[ ${monthCount} -gt 2]];then
    toDelete=$((monthCount -2))
    for file in $(find "${DIR}/1" -mindepth 1 -name "${object}*" -type f | sort | head -n ${toDelete});do
       rm ${file}
      usage=$(df ${DIR} | tail -n1 | awk '{print $5}' | sed -E 's/%$//')
                        if [[ ${usage} - le ${NUMBER} ]] ; then
                                exit 0
                        fi
                done
        elif [[ ${weeklyCount} -gt 3 ]] ; then
                toDelete=$(( weeklyCount - 3 ))
                for file in $(find "${DIR}/2" -mindepth 1 -name "${object}*" -type f | sort | head -n ${toDelete}) ; do
                        rm ${file}
                        usage=$(df ${DIR} | tail -n1 |awk '{print $5}' | sed -E 's/%$//')
                        if [[ ${usage} -le ${NUMBER} ]] ; then
                                exit 0
                        fi
                done
        elif [[ ${dailyCount} -gt 4 ]] ; then
                toDelete=$(( dailyCount - 4 )) ; then
                for file in $(find "${DIR}/3" -mindepth 1 -name "${object}*" -type f | sort | head -n ${toDelete}) ; do
                        rm ${file}
                        usage=$(df ${DIR} | tail -n1 | awk '{print $5}' | sed -E 's/%$//')
                        if [[ ${usage} - le ${NUMBER} ]] ; then
                                exit 0
                        fi
                done
        fi

  find "${DIR}" -mindepth 1 -type l -exec test ! -e {} \; -print | xargs -I{} rm {}
  
