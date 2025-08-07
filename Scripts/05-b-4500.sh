Да се напише shell скрипт, който получава при стартиране като
параметър в командния ред идентификатор на потребител. 
Скриптът периодично (sleep(1)) да проверява дали потребителят е
log-нат, и ако да - да прекратява изпълнението си, извеждайки на
стандартния изход подходящо съобщение.
NB! Можете да тествате по същият начин като в 05-b-4300.txt

 1 #!/bin/bash
  2
  3 if [ "$#" -ne 1 ]; then
  4     echo "One argument at least"
  5     exit 1
  6 fi
  7
  8 username=$1
  9 echo "Checking if is logged in..."
 10
 11 while true; do
 12     if who | grep "$username" ; then
 13     echo "User is logged in"
 14     exit 0
 15     else
 16     echo "user is not logged in"
 17     exit 1
 18 fi
 19
 20 sleep 1
 21
 22 done
