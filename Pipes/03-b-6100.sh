#Отпечатайте последната цифра на UID на всички редове между 28-ми и 46-ред в /etc/passwd.

$cat /etc/passwd | head -n 46 | tail -n 19 | cut -f 3 -d ':' | rev | cut -c 1
