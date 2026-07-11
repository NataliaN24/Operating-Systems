group=$(sort -t: -k3n /etc/passwd | head -n 201 | tail -n 1 | cut -d: -f4)

grep ":$group:" /etc/passwd | cut -d: -f1,5,6 | sort -t: -k1 | cut -d: -f2,3

##############################################################


cat /etc/passwd |
sort -t ':' -k3,3n |
head -n 201 |
tail -n 1 |
cut -d ':' -f4 |
xargs -I{} grep ":{}:" /etc/passwd |
sort -t ':' -k1.2,1n |
cut -d ':' -f5,6 |
sed 's/,.*:/:/'
