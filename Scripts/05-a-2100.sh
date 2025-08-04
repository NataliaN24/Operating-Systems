#Редактирайте вашия .bash_profile файл, за да ви поздравява (или да изпълнява някаква команда по ваш избор) всеки път, когато влезете в системата.

nano ~/.bash_profile
echo "Добре дошъл/дошла, $USER! Днес е $(date +'%A, %d %B %Y')"

#or
  1 #!/bin/bash
  2
  3 name=$(whoami)
  4 echo "Hello $name"

#or
#!/bin/bash

name=$(grep -e "^$(whoami)" /etc/passwd | awk -F ':' '{print $5}' | awk '{print $1}')
 echo "Hello $name"
