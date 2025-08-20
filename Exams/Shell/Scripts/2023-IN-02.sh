Вашите колеги от съседната лаборатория ползват специализиран софтуер за оптометрични изследвания, който записва резултатите от всяко измерване в отделен файл. Файловете имат уникално съдържание, по което се определя за кое измерване се отнася файла. За съжаление, тъй като колегите
ви ползват бета версия на софтуера, той понякога записва по няколко пъти резултатите от дадено измерване в произволна комбинация от следните варианти:
• нула или повече отделни обикновенни файла с еднакво съдържание;
• нула или повече групи от hardlink-ове, като всяка група съдържа две или повече имена на даден
файл с измервания.
Помогнете на колегите си, като напишете shell скрипт, който приема параметър – име на директория,
съдържаща файлове с измервания. Скриптът трябва да извежда на стандартния изход списък с имена
на файлове, кандидати за изтриване, по следните критерии:
• ако измерването е записано само в отделни файлове, трябва да остане един от тях;
• ако измерването е записано само в групи от hardlink-ове, всяка група трябва да се намали с едно
име;
• ако измерването е записано и в групи, и като отделни файлове, за групите се ползва горния
критерий, а всички отделни файлове се премахват.

#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <directory>"
    exit 1
fi

DIR="$1"

if [[ ! -d "$DIR" ]]; then
    echo "Error: $DIR is not a directory"
    exit 1
fi

# 1. Намираме всички файлове (обикновени + hardlinks) и групираме по inode
# inode идентифицира реалния файл на файловата система
find "$DIR" -type f -printf "%i %p\n" | sort -n > /tmp/files_inodes.txt

# 2. Създаваме асоциативен масив: ключ = inode, стойност = списък от файлове
declare -A inode_files
while read inode file; do
    inode_files[$inode]="${inode_files[$inode]} $file"
done < /tmp/files_inodes.txt

# 3. Създаваме масив за уникално съдържание: ще ползваме checksum (md5sum)
declare -A content_inodes

for inode in "${!inode_files[@]}"; do
    # Взимаме един от файловете на inode за checksum
    file=$(echo ${inode_files[$inode]} | awk '{print $1}')
    sum=$(md5sum "$file" | awk '{print $1}')
    # Добавяме inode към списъка на този checksum
    content_inodes[$sum]="${content_inodes[$sum]} $inode"
done

# 4. Обработваме всеки уникален content
for sum in "${!content_inodes[@]}"; do
    # inode-ите, които имат това съдържание
    inodes=(${content_inodes[$sum]})

    # файловете за този content
    declare -a all_files=()
    for inode in "${inodes[@]}"; do
        files=(${inode_files[$inode]})
        all_files+=("${files[@]}")
    done

    # Разделяме на hardlink групи и единични файлове
    single_files=()
    group_files=()
    for inode in "${inodes[@]}"; do
        files=(${inode_files[$inode]})
        if [[ ${#files[@]} -eq 1 ]]; then
            single_files+=("${files[0]}")
        else
            group_files+=("${files[@]}")
        fi
    done

    # 5. Определяме файлове за изтриване

    # ако има и групи, и единични файлове -> всички единични файлове се изтриват
    if [[ ${#group_files[@]} -gt 0 ]] && [[ ${#single_files[@]} -gt 0 ]]; then
        for f in "${single_files[@]}"; do
            echo "$f"
        done
        single_files=()  # зануляваме, за да не се обработят повторно
    fi

    # за групите -> оставяме едно име, изтриваме останалите
    for inode in "${inodes[@]}"; do
        files=(${inode_files[$inode]})
        if [[ ${#files[@]} -gt 1 ]]; then
            for ((i=1;i<${#files[@]};i++)); do
                echo "${files[i]}"
            done
        fi
    done

    # ако няма групи, само единични -> оставяме едно име, изтриваме останалите
    if [[ ${#group_files[@]} -eq 0 ]] && [[ ${#single_files[@]} -gt 1 ]]; then
        for ((i=1;i<${#single_files[@]};i++)); do
            echo "${single_files[i]}"
        done
    fi
done

# cleanup
rm -f /tmp/files_inodes.txt

#or solution 2

#!/bin/bash

[[ $# -ne 1 ]] && echo "1 param expected" && exit 1
[[ ! -d $1 ]] && echo "the 1st param must be a dir" && exit 2

files=$(find $1 -type f -printf '%i %n %p\n')

DIR=$(mktemp -d)
while read inode count name; do
	sha=$(sha256sum $name | cut -d ' ' -f 1)

	echo "$inode $count" >> $DIR/$sha
done < <(echo "$files")

files=$(find $DIR -type f)

while read sha; do
	file=$(cat $sha | sort -u)

	ones=$(echo "$file" | egrep " 1$" | cut -d ' ' -f 1)
	notOnes=$(echo "$file" | egrep -v " 1$" | cut -d ' ' -f 1)

	if [[ -z $notOnes ]]; then
		toDelete=$(echo "$ones" | tail -n +2)
		
		[[ -z $toDelete ]] && echo "File with inode number $ones is just 1" && continue

		while read inode; do
			find $1 -type f -inum "$inode"
		done < <(echo "$toDelete")
	elif [[ -z $ones ]]; then
		while read inode; do
			find $1 -type f -inum "$inode" | head -n 1
		done < <(echo "$notOnes")
	else
		while read inode; do
			find $1 -type f -inum "$inode"
		done < <(echo "$ones")

		while read inode; do
			find $1 -type f -inum "$inode" | head -n 1
		done < <(echo "$notOnes")
	fi
done < <(echo "$files")

rm -r $DIR
