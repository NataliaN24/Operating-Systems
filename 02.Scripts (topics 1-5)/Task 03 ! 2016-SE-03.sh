#!/bin/bash

if [[ $# -ne 0 ]]; then
    exit 1
fi

if [[ $(id -u) -ne 0 ]]; then
    exit 1
fi

while IFS=: read -r name pass uid gid info homedir shell; do

    if [[ ! -d "$homedir" ]]; then
        echo "$name $uid"
        continue
    fi

    owner_uid=$(stat -c '%u' "$homedir")
    owner_perm=$(stat -c '%A' "$homedir" | cut -c 3)

    if [[ "$uid" == "$owner_uid" && "$owner_perm" != "w" ]]; then
        echo "$name $uid"
    fi

done < /etc/passwd


##########################################################

#!/bin/bash
if [[ $# -ne 0 ]];then
  echo "no need for arg""
  exit 1
fi

if [[ "$(id -u)" -ne 0 ]];then
  echo "must be root"
  exit 2
fi
#we extract only the first and sicth field ,while the field separator is : we split these two columns
#into user col uid col gid col  and home col and for each of them we check if the dir is "home"
#if not we have found it 

cut -d ":" -f1,3,4,6 /etc/passwd | while IFS=: read -r user uid gid home;do
#do not have homedir
  if [[ ! -d "$home" ]]; then
    echo "$user has no homedir: $home "
    continue
  fi
read -r h_uid h_gid perm < <(stat -c '%u %g %a' "$home" 2>/dev/null)
if [[ -z "$perm" ]];then
  echo "$user :cannot stat $home"
  continue;
fi
#${variable:offset:length}
owner=${perm:0:1}
group=${perm:1:1}
other=${perm:2:1}

 #by default we accept that user is others bc if he is not owner and not in group linux will se in others
  #1. Ако си owner → гледа owner
  #2. Иначе ако си в group → гледа group
  #3. Иначе → гледа others
  
 digit=$other
 if [[ "$uid" -eq "$h_uid" ]];then
   digit=$owner
  elif [[ "$gid" -eq "$h_gid" ]];then
    digit=$group
  fi
  
if ! ((digit & 2)); then
  echo "$user cannot write in $home"
fi



  #-r казва на read:Не интерпретирай backslash-и
