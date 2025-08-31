# Изведете имената на потребителите, които имат поне 2 процеса, 
#чиято команда е vim (независимо какви са аргументите й)

$ ps -eo user,cmd | grep 'vim' | awk '{count[$1]++} END {for ( i in count ) if ( count[i] >=2) print i}'
