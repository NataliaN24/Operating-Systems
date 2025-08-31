#Изведете командата на най-стария процес

ps -e --sort=etime -o cmd | tail -n 1
#or
ps -e -o comm= --sort=etimes | tail -n 1
