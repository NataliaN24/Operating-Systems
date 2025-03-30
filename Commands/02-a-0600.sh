#Създайте директорията practice/01 във вашата home директория.
#Създайте 3 файла в нея - f1, f2, f3.
#Копирайте файловете f1, f2, f3 от директорията practice/01/ в директория dir1, намираща се във вашата home директория. Ако нямате такава, създайте я.

mkdir -p practice/01
cd practice/01
touch f1
touch f2
touch f3
cd ~
mdkir dir1
cd practice/01
cp ~/practice/01/f1 ~/practice/01/f2 ~/practice/01/f3 ~/dir1/
