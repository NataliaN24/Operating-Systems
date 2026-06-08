#!/bin/bash

# 1) Проверка за конфигурационния файл
if [[ -z "${ARKCONF}" ]]; then
    echo "Missing ARKCONF"
    exit 1
fi

if [[ ! -f "${ARKCONF}" ]]; then
    echo "Config file does not exist"
    exit 1
fi

# 2) Зареждаме конфигурацията
source "${ARKCONF}"

# 3) Проверка дали има WHAT, WHERE, WHO
if [[ -z "${WHAT}" || -z "${WHERE}" || -z "${WHO}" ]]; then
    echo "Missing config values"
    exit 1
fi

# 4) WHAT трябва да е абсолютен път до директория
if [[ "${WHAT}" != /* ]]; then
    echo "WHAT must be absolute path"
    exit 1
fi

if [[ ! -d "${WHAT}" ]]; then
    echo "WHAT must be existing local directory"
    exit 1
fi

# добавяме / накрая, ако няма
if [[ "${WHAT}" != */ ]]; then
    WHAT="${WHAT}/"
fi

# 5) Четем аргументите
direction=""
deleteOpt=""
server=""

for arg in "$@"; do
    if [[ "${arg}" == "push" || "${arg}" == "pull" ]]; then
        if [[ -n "${direction}" ]]; then
            echo "Only one of push/pull is allowed"
            exit 1
        fi
        direction="${arg}"
    elif [[ "${arg}" == "-d" ]]; then
        deleteOpt="--delete"
    else
        if [[ -n "${server}" ]]; then
            echo "Only one server can be given"
            exit 1
        fi
        server="${arg}"
    fi
done

# 6) Трябва да има точно една подкоманда
if [[ -z "${direction}" ]]; then
    echo "Missing push or pull"
    exit 1
fi

# 7) Ако е подаден server, проверяваме дали е в WHERE
if [[ -n "${server}" ]]; then
    found=0
    for s in ${WHERE}; do
        if [[ "${s}" == "${server}" ]]; then
            found=1
            break
        fi
    done

    if [[ ${found} -eq 0 ]]; then
        echo "Invalid server"
        exit 1
    fi

    servers="${server}"
else
    servers="${WHERE}"
fi

# 8) Синхронизация
for s in ${servers}; do

    if [[ "${direction}" == "push" ]]; then
        SRC="${WHAT}"
        DEST="${WHO}@${s}:${WHAT}"
    else
        SRC="${WHO}@${s}:${WHAT}"
        DEST="${WHAT}"
    fi

    echo "-----------------------------------"
    echo "Server: ${s}"
    echo "Preview:"
    rsync -a -v ${deleteOpt} -n "${SRC}" "${DEST}"

    echo -n "Continue? [y/N] "
    read answer

    if [[ "${answer}" == "y" || "${answer}" == "Y" ]]; then
        rsync -a -v ${deleteOpt} "${SRC}" "${DEST}"
    else
        echo "Skipped ${s}"
    fi
done
