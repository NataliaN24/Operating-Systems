#Намерете командите на 10-те най-стари процеси в системата.

ps -e --sort=etimes -o cmd | tail -n 10
#or
ps -A --sort=etime -o cmd | tail -n 10
