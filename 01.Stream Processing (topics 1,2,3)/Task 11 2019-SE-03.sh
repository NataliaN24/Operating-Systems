inode=$( find /home/velin -type f -printf '%T@ %i\n' | sort -nr | head -n 1 |cut -d ' ' -f2)

find /home/velin -type f -inum "$inode" -printf '%p\n' 2>/dev/null | tr -cd '/' | wc -c | sort -n | head -n1
