#Във file3 (inplace) изтрийте всички "1"-ци.

 tr -d "1" < file3 > temp && mv temp file3
