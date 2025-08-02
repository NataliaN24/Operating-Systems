# Използвайки файл population.csv, намерете през коя година в България има най-много население.

cat 03-b-9050 | egrep "Bulgaria" | sort -t ',' -k4 | tail -n 1 | cut -f3 -d ","
