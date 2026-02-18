find "$HOME" -type f -maxdepth 1 -user $(whoami) -exec chmod 664 {} \;
