#Намерете файловете в /bin, които могат да се четат, пишат и изпълняват от всички.
find /bin -perm 777

#second solution
 find /bin -perm -u=rwx,g=rwx,o=rwx

