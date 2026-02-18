mostFailuresSite=$(tail -n +2 spacex.txt | cut -d '|' -f2,3 | grep 'Failure$' | cut -d '|' -f1 | sort | uniq -c | sort -nr | head -n 1 | cut -d ' ' -f2)
tail -n +2 spacex.txt| grep "|$mostFailuresSite|" | sort -t '|' -k1,1nr | head -n 1 | cut -d '|'-f3,4 | tr '|' ':' 
