#Изтрийте файловете в home директорията си по-нови от practice/01/f3
#(подайте на rm опция -i за да може да изберете само тези които искате да изтриете).

#first solution
find ~ -newer practice/01/f3 -exec rm -i {} \;

 #second solution
 find ~ -newer practice/01/f3 -type f -delete
