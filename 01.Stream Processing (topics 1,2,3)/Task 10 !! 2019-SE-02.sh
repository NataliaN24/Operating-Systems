grep '/home/SI/' /etc/passwd | while IFS=: read -r user pass uid gid gecos home shell;do
  status=$(stat -c '%Z' "$home")
  if [[ "$status" -ge 1551168000 && "$status" -le 1551176100 ]]; then
    fn="{user#s}"
    name=${gecos%%,*}" #Маха всичко от първата запетая нататък
    prints '%s\t%s\n' "$fn" "$name"
  fi
done
