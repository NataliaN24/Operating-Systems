Създайте следната файлова йерархия в home директорията ви:
dir5/file1
dir5/file2
dir5/file3

Посредством vi въведете следното съдържание:
file1:
1
2
3

file2:
s
a
d
f

file3:
3
2
1
45
42
14
1
52


mkdir dir5
touch dir5/file1 dir5/file2 dir5/file3

Изведете на екрана:
	* статистика за броя редове, думи и символи за всеки един файл
	
wc file1 file2 file 3 
#by default wc shows the number of chars ,symbols and words and total of the three options

* статистика за броя редове и символи за всички файлове

wc file1 file2 file3 | tail -n 1 | awk '{print $1,$3}'
#or 
 wc -l -m file1 file2 file3 | tail -n 1

	* общия брой редове на трите файла
  wc -l file1 file2 file3 | tail -n 1

