#Колко файлове в /bin са 'shell script'-oве? (Колко файлове в дадена директория са ASCII text?)

$ find /bin -type f -exec file {} \; | grep "shell script" | wc -l

$find /bin | xargs -I{} file {} | grep "ASCII text executable" | wc -l
