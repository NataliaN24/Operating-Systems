#Във file3 (inplace) изтрийте всички "1"-ци.

$ tr -d "1" < file3 > temp && mv temp file3

 #or
 $sed -i 's/1//g' dir5/file3
