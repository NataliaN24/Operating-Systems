Опишете поредица от команди или напишете shell скрипт, които/който при известни две
директории SOURCE и DESTINATION:
- намира уникалните "разширения" на всички файлове, намиращи се някъде под SOURCE.
(За простота приемаме, че в имената на файловете може да се среща символът точка '.' 
максимум веднъж.)
- за всяко "разширение" създава по една поддиректория на DESTINATION със същото име
- разпределя спрямо "разширението" всички файлове от SOURCE в съответните поддиректории
в DESTINATION

1 #!/bin/bash
  2
  3
  4 if [ $# -ne 2 ]; then
  5     exit 1
  6 fi
  7
  8 sourceDir=$1
  9 dest=$2
 10
 11 if [ ! -d "$sourceDir" ] || [ ! -d "$dest" ]; then
 12     exit 1
 13 fi
 14
 15 for file in $(find "$sourceDir" -type f); do
 16     extension=$(echo "$file" | awk -F '.' '{print $NF}')
 17     echo "$dest/$extension"
 18     if [ ! -d "$dest/$extension" ]; then
 19          echo "make"
 20          mkdir "$dest/$extension"
 21     fi
 22     cp "$file" "$dest/$extension"
 23 done
