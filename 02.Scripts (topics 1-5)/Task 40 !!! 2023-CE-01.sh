[[ $# -eq 2 ]] || exit 1
file=$1
starType=$2
[[ -f "$file" ]] || exit 1

awk -F',' -v t="$starType" 
  '$5==t {cnt[$4]++}
  END {
  max =-1
  for (c in cnt ) if (cnt[c]>max) {max=cnt[c]; best=c}
  if(max<0) exit 2
  print best
  }
  ' "$file"  |
  while IFS= read -r bestCount;do
    awk -F',' -v t="$starType" -v c="$bestConst" '
    $5==t && $4==c  && $7 !="--" {
    m=$7+0    #converting to number
    if (!seen || m<bestmm){
    bestm=m;
    bestname=$1;
    seen=1;}
    END {
    if (seen) print bestname;else exit 3
    }' "$file"
  done
    
