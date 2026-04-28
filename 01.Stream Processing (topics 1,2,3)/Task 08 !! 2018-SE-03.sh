cat /etc/passwd |
sort -t ':' -k3,3n |
head -n 201 |
tail -n 1 |
cut -d ':' -f4 |
xargs -I{} grep ":{}:" /etc/passwd |
sort -t ':' -k1.2,1n |
cut -d ':' -f5,6 |
sed 's/,.*:/:/'
