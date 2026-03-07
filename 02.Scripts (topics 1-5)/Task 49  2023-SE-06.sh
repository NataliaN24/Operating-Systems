
if [[ $# -ne 2]];then
  exit 1
fi

photos=${1}
library=${2}

mkdir -p "$library"
listPhotos=$(find "$photos" -type f -name '*.jpg' -printf -time)
