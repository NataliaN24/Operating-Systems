cut -d: -f1,5 /etc/passwd |
grep "Inf" |
grep -E "[A-Za-z]+a," |
cut -d: -f1 |
cut -c3-4 |
sort |
uniq -c |
sort -nr |
head -n1



###################################################
 grep home/Inf/ test2017 | cut -d : -f1,5 | tr ':' ' '  | tr ',' ' ' | cut -d ' ' -f1,3 | egrep '.*a$' | cut -d ' ' -f1 | cut  -c 2-3 | sort | uniq -c | sort -nr | tr -s ' ' | sed 's/ //' | head -n 1 | cut -d ' ' -f2
