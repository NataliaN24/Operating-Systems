#!/bin/bash

user=$(whoami)
if [[ $user != "oracle" && $user != "grid" ]];then
  exit 1
fi
if [[ $# -ne 1]];then
  exit 2;
fi
if [[ ! $1 =~ ^[0-9]+$ ]]; then
  exit 2
fi

machine=$(hostname)

if [[ -z $ORACLE_BASE ]];then 
  exit 2;
fi
if [[ -z$ORACLE_HOME ]];then 
  exit 2;
fi
if [[ -z $ORACLE_SID ]];then 
  exit 2;
fi

sqlplus=$(find $ORACLE_HOME/bin/sqlplus -executable)
if [[ -z $sqlplus ]]; then
  exit 4;
fi

role="SYSDBA"
if [[ $user == "grid"]]; then
  role="SYSASM"
fi
sqlplusOutput=$($sqlplus -SL "/ as $role " @foo.sql)
exitStatus=$?

diag_base="$ORACLE_HOME"
if [[ $exitStatus -eq 0  ]];then
  diag_base=$(echo $sqlplusOutput | tail -n +4 | head -n1)
fi

diag_dir="$diag_base/diag"
if [[ ! -d $diag_dir ]]; then
  exit 6
fi

fileEnd=".*_[0-9]+\."
trcTrm="${fileEnd}(trc|trm)$"
log="${fileEnd}log$"
xml="${fileEnd}xml$"

crsPath="diag_dir/crs/$machine/crs/trace"
crsFiles=$(find $crsPath -type f -mtime +$1 -printf '%s %p\n' |egrep "$trcTrm | cut -d ' ' -f1)
size=$(echo "$crsFiles | awk '{ sum += $0} END { print sum/1024}')
echo "CRS: $size"

tnslsnrPath="$diag_dir/tnslsnr/$machine"
tnslsnrFiles=$(find $tnslsnrPath -mindepth 3 -maxdepth 3 -type f -path "$tnslsnrPath/*/alert/*" -o -path "$tnslsnrPath/*/trace/*" -mtime +$1 -printf '%s %p\n' | egrep "(alert/$xml|trace/$log)" | cut -d ' ' -f 1)
size=$(echo "$tnslsnrFiles" | awk '{ sum += $0 } END {print sum/1024}')

echo "TNSLSNR: $size"

rdbmsPath="$diag_dir/rdbms"
rdbmsFiles=$(find $rdbmsPath -mindepth 2 -maxdepth 2 -type f -mtime +$1 -printf '%s %p\n' | egrep "$trcTrm" | cut -d ' ' -f 1)
size=$(echo "$rdbmsFiles" | awk '{ sum += $0 } END {print sum/1024}')

echo "RDBMS: $size"
