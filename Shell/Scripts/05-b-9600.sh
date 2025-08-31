Да се напише shell скрипт, който получава произволен брой аргументи файлове, които изтрива.
Ако бъде подадена празна директория, тя бива изтрита. Ако подадения файл е директория с поне 1 файл, тя не се изтрива.

Да се дефинира променлива BACKUP_DIR (или друго име), в която:
- изтритите файлове се компресират и запазват
- изтритите директории се архивират, комприсират и запазват
- имената на файловете е "filename_yyyy-mm-dd-HH-MM-SS.{gz,tgz}", където filename е оригиналното име на файла (директорията) преди да бъде изтрит

а) Добавете параметър -r на скрипта, който позволява да се изтриват непразни директории рекурсивно и съответно да се запазят в BACKUP_DIR

Примери:
$ export BACKUP_DIR=~/.backup/

# full-dir/ има файлове и не може да бъде изтрита без параметър -r
$ ./trash f1 f2 full-dir/ empty-dir/
error: full-dir/ is not empty, will not detele
$ ls $BACKUP_DIR
f1_2018-05-07-18-04-36.gz
f2_2018-05-07-18-04-36.gz
empty-dir_2018-05-07-18-04-36.tgz

$ ./trash -r full-dir/

$ ls $BACKUP_DIR
f1_2018-05-07-18-04-36.gz
f2_2018-05-07-18-04-36.gz
full-dir_2018-05-07-18-04-50.tgz
empty-dir_2018-05-07-18-04-36.tgz

# можем да имаме няколко изтрити файла, които се казват по един и същ начин
$ ./trash somedir/f1

$ ls $BACKUP_DIR
f1_2018-05-07-18-04-36.gz
f1_2018-05-07-18-06-01.gz
f2_2018-05-07-18-04-36.gz
full-dir_2018-05-07-18-04-50.tgz
empty-dir_2018-05-07-18-04-36.tgz

#!/bin/bash

# Проверка за променлива BACKUP_DIR
if [ -z "$BACKUP_DIR" ]; then
    echo "Error: BACKUP_DIR is not set"
    exit 1
fi

# Проверка дали BACKUP_DIR съществува, ако не - създаваме
if [ ! -d "$BACKUP_DIR" ]; then
    mkdir -p "$BACKUP_DIR" || { echo "Cannot create backup directory"; exit 1; }
fi

# Флаг за рекурсивно изтриване на непразни директории
recursive=false

# Ако първият аргумент е -r, слагаме флага и махаме аргумента
if [ "$1" == "-r" ]; then
    recursive=true
    shift
fi

timestamp() {
    date '+%Y-%m-%d-%H-%M-%S'
}

backup_file() {
    local filepath="$1"
    local filename=$(basename "$filepath")
    local ts=$(timestamp)
    local dest="$BACKUP_DIR/${filename}_$ts.gz"
    gzip -c "$filepath" > "$dest" || { echo "Failed to backup $filepath"; return 1; }
}

backup_dir() {
    local dirpath="$1"
    local dirname=$(basename "$dirpath")
    local ts=$(timestamp)
    local dest="$BACKUP_DIR/${dirname}_$ts.tgz"
    tar czf "$dest" -C "$(dirname "$dirpath")" "$dirname" || { echo "Failed to backup directory $dirpath"; return 1; }
}

process_path() {
    local path="$1"

    if [ ! -e "$path" ]; then
        echo "error: $path does not exist"
        return
    fi

    if [ -f "$path" ]; then
        # Файл - архивирай и изтрий
        backup_file "$path" && rm "$path" && echo "Deleted file $path"
    elif [ -d "$path" ]; then
        # Директория
        if [ "$recursive" = true ]; then
            # Рекурсивно изтриване и архивиране
            backup_dir "$path" && rm -r "$path" && echo "Deleted directory recursively $path"
        else
            # Проверка дали е празна
            if [ -z "$(ls -A "$path")" ]; then
                # Празна директория - архивирай и изтрий
                backup_dir "$path" && rmdir "$path" && echo "Deleted empty directory $path"
            else
                echo "error: $path is not empty, will not delete"
            fi
        fi
    else
        echo "error: $path is not a file or directory"
    fi
}

if [ $# -lt 1 ]; then
    echo "Usage: $0 [-r] file|directory ..."
    exit 1
fi

for item in "$@"; do
    process_path "$item"
done
