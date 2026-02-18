find /var/log/my_logs -type f -name "*.log" 2>/dev/null | grep -E '/[A-Za-z0-9_]+_[0-9]+\.log$' | xargs grep -o 'error' 2>/dev/null | wc -l
