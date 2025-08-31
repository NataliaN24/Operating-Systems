#Използвайки файл population.csv, намерете коя държава има най-много население през 2016.
# А коя е с най-малко население?

#not the best options
cat 03-b-9050 | egrep "2016" | sort -t ',' -k4 -n | tail -n 1 | cut -f 1 -d ","

cat 03-b-9050 | egrep "2016" | sort -t ',' -k4 -n | head -n 1 | cut -f 1 -d ","

#or
grep ",2016," population.csv | cut -d '"' -f3 | sort -k 4n -t ',' | tail -n 1 | cut -d ',' -f2 | xargs -I{} grep {} population.csv | head -n 1 | rev | cut -d ',' -f4 | rev | tr -d '"'
