#от предната задача: когато вече сте получили myetc с файлове, архивирайте
#всички от тях, които започват с 'c' в архив, който се казва c_start.tar
#изтрийте директорията myetc и цялото и съдържание
#изтрийте архива c_start.tar

// solution from assistant
tar -c -f c_start.tar $(find myetc/ -iname 'c*') 
# or tar -caf c_start.tar.zst /etc/c*
rm -rf myetc/
rm -i c_start.tar

//my test solution
#created a test directory with a test file 

 tar -cf testArchive.tar ./test_archive
 tar -tf testArchive.tar  # to see the content
 rm  testArchive.tar
