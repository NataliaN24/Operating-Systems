#За всяка група от /etc/group изпишете "Hello, <група>", като ако това е вашата група, 
#напишете "Hello, <група> - I am here!".

awk -F: -v mygroup="$(id -gn)" '
{
  if ($1 == mygroup) 
    print "Hello, " $1 " - I am here!";
  else 
    print "Hello, " $1;
}
' /etc/group
