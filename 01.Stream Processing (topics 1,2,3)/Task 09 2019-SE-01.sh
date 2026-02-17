type=$(tail -n +2 planets.txt | sort -t ';' -k3,3n | tail -n 1 | cut -f2 -d';')

tail -n +2 planets.txt | grep -F ";$type;" | sort -t ';' -k3,3n | head -n 1 |cut -d';'-f1,4 | tr ';' '\t'
