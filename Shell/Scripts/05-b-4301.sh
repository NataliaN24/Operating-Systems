#Напишете shell script, който автоматично да попълва файла указател от предната задача по подадени аргументи:
#име на файла указател, пълно име на човека (това, което очакваме да е в /etc/passwd) и избран за него nickname.
#Файлът указател нека да е във формат:
#<nickname, който лесно да запомните> <username в os-server>
#// може да сложите и друг delimiter вместо интервал
#
#Примерно извикване:
#./pupulate_address_book myAddressBook "Ben Dover" uncleBen
#
#Добавя към myAddressBook entry-то:
#uncleBen <username на Ben Dover в os-server>
#
#***Бонус: Ако има няколко съвпадения за въведеното име (напр. има 10 човека Ivan Petrov в /etc/passwd),
#всички те да се показват на потребителя, заедно с пореден номер >=1,
#след което той да може да въведе някой от номерата (или 0 ако не си хареса никого), и само избраният да бъде добавен
#към указателя.

#!/bin/bash
# This is a Bash script to add a user entry to an address book based on full name and nickname.

if [[ $# -ne 3 ]] ; then
    # Check if exactly 3 arguments are passed; if not, show error and exit.
    echo "Expected 3 arguments"
    exit 1
fi

fileName=${1}     # First argument: file where the entry will be stored
fullName=${2}     # Second argument: full name to search for in /etc/passwd
nickName=${3}     # Third argument: nickname to be used in the address book

# Search for all users whose full name matches the given fullName in /etc/passwd
# awk is used to extract the username (field 1)
userName=$(grep "${fullName}" /etc/passwd | awk -F ':' '{print $1}')

# Count how many matching usernames were found
candidates=$(echo "${userName}" | wc -l)

if [[ ${userName} == "" ]] ; then
    # If no user was found, exit with message
    echo "User not found"
    exit 1
fi

if [[ ${candidates} -gt 1 ]] ; then
    # If there are multiple matches, ask user to choose one
    index=1
    while read line ; do
        # Display each username with a number
        echo "${index}.${line}"
        arr[index]=${line}  # Store in an array for later selection
        index=$(( index + 1 ))  # Increment index
    done < <(echo "${userName}")  # Read the list of usernames

    echo "0.Exit"  # Option to cancel
    read -p "Choose from 1-${candidates}: " choice  # Prompt for user selection

    if [[ ${choice} -lt 0 ]] || [[ ${choice} -gt ${candidates} ]] ; then
        # If choice is not valid (out of range), exit
        echo "Invalid choice"
        exit 1
    else
        if [[ ${choice} -eq 0 ]] ; then
            # User chose to cancel
            exit 0
        else
            # Set the selected username
            userName=${arr[${choice}]}
        fi
    fi
fi

# Append the nickname and selected username to the address book file
echo "${nickName} ${userName}" >> ${fileName}
