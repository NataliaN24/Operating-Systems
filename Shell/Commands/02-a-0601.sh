#Нека файлът f2 бъде преместен в директория dir2, намираща се
#във вашата home директория и бъде преименуван на numbers.

# 1st solution

mkdir dir2
mv ~/dir1/f2 ~/dir2/
cd dir2
mv f2 numbers

#2nd solution

mkdir -p ~/dir2
mv ~/practice/01/f2 ~/dir2/numbers
