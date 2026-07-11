grep '/home/SI' /etc/passwd | while IFS=: read user x uid gid name home shell
do
    ctime=$(stat -c %Z "$home")
    if [ "$ctime" -ge 1551168000 ] && [ "$ctime" -le 1551176100 ]
    then
        echo -e "${user#s}\t$name"
    fi
done
