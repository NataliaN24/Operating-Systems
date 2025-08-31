#Във file2 (inplace) подменете всички малки букви с главни.

 tr 'a-z' 'A-Z' < file2 > temp && mv temp file2
