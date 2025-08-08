Да се напише shell script, който генерира HTML таблица съдържаща описание на
потребителите във виртуалката ви. Таблицата трябва да има:
- заглавен ред с имената нa колоните
- колони за username, group, login shell, GECOS field (https://en.wikipedia.org/wiki/Gecos_field)

Пример:
$ ./passwd-to-html.sh > table.html
$ cat table.html
<table>
  <tr>
    <th>Username</th>
    <th>group</th>
    <th>login shell</th>
    <th>GECOS</th>
  </tr>
  <tr>
    <td>root</td>
    <td>root</td>
    <td>/bin/bash</td>
    <td>GECOS here</td>
  </tr>
  <tr>
    <td>ubuntu</td>
    <td>ubuntu</td>
    <td>/bin/dash</td>
    <td>GECOS 2</td>
  </tr>


  1 #!/bin/bash
  2
  3 users=$(who | awk '{print $1}' | sort -u)
  4
  5 echo "<table>"
  6 echo "  <tr>"
  7 echo "    <th>Username</th>"
  8 echo "    <th>group</th>"
  9 echo "    <th>login shell</th>"
 10 echo "    <th>GECOS</th>"
 11 echo "  </tr>"
 12
 13 while read username ; do
 14         echo "    <tr>"
 15         groupId=$(egrep "^${username}" /etc/passwd | awk -F ':' '{print $4}')
 16         group=$(egrep "${groupId}:$" /etc/group | awk -F ':' '{print $1}')
 17         loginShell=$(egrep "^${username}" /etc/passwd | awk -F ':' '{print $NF}')
 18         fullname=$(egrep "^${username}" /etc/passwd | awk -F ':' '{print $5}' | awk -F ',' '{print
 19         echo "      <td>${username}</td>"
 20         echo "      <td>${group}</td>"
 21         echo "       <td>${loginShell}</td>"
 22         echo "      <td>${fullname}</td>"
 23         echo "     </tr>"
 24 done < <(echo "${users}")
 25 echo   "</table>"

</table>
