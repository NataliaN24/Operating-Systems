#Изведете имената на хората с второ име по-късо от 8 (<=7) символа според /etc/passwd // !(>7) = ?

  #first option
  cat /etc/passwd | cut -f 5 -d ':' | cut -f 2 -d ' ' | cut -f 1 -d ','| grep -E '^.{1,7}$'
  
  #second option
  cat /etc/passwd | cut -d ':' -f 5 | cut -d ',' -f1 | cut -d ' ' -f2 | grep -v -E '.{8,}$'
