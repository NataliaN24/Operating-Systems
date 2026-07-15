#!/bin/bash

repo="$1"
pkg="$2"

# Проверка на аргументите
if [[ $# -ne 2 ]]; then
    exit 1
fi

if [[ ! -d "$repo" ]]; then
    exit 1
fi

if [[ ! -d "$pkg" ]]; then
    exit 1
fi

# Име на пакета
pkgName=$(basename "$pkg")

# Версия
version=$(cat "$pkg/version")

# Директория tree
treeDir="$pkg/tree"

# Създаване на архив на съдържанието на tree
tmpArchive=$(mktemp)

tar -cJf "$tmpArchive" -C "$treeDir" .

# Изчисляване на checksum
hashSum=$(sha256sum "$tmpArchive" | cut -d ' ' -f1)

# Път до db
db="$repo/db"

# Проверка дали съществува същата версия
oldLine=$(grep "^$pkgName-$version " "$db")

if [[ -n "$oldLine" ]]; then
    
    # Стар checksum
    oldHash=$(echo "$oldLine" | cut -d ' ' -f2)

    # Изтриваме стария архив
    rm -f "$repo/packages/$oldHash.tar.xz"

    # Замяна на реда в db
    sed -i "s/^$pkgName-$version .*/$pkgName-$version $hashSum/" "$db"

else

    # Добавяне на нов пакет
    echo "$pkgName-$version $hashSum" >> "$db"

fi

# Сортиране на db
sort "$db" -o "$db"

# Преместване на архива в packages
mv "$tmpArchive" "$repo/packages/$hashSum.tar.xz"

////////////////////////////////////////////////////////////////////////////////////
#!/bin/bash

if [[ $# -ne 2 ]]; then
    exit 1
fi

repo="$1"
repoDb="$repo/db"
repoPkg="$repo/packages"

pkg="$2"
pkgVer="$pkg/version"
pkgTree="$pkg/tree"

if [[ ! -d "$repo" ]]; then
    exit 1
fi

if [[ ! -f "$repoDb" ]]; then
    exit 1
fi

if [[ ! -d "$repoPkg" ]]; then
    exit 1
fi

if [[ ! -d "$pkg" ]]; then
    exit 1
fi

if [[ ! -f "$pkgVer" ]]; then
    exit 1
fi

if [[ ! -d "$pkgTree" ]]; then
    exit 1
fi

pkgname=$(basename "$pkg")
version=$(cat "$pkgVer")
fullName="${pkgname}-${version}"

tmpArchive=$(mktemp)

tar -cJf "$tmpArchive" -C "$pkgTree" .

checksum=$(sha256sum "$tmpArchive" | cut -d ' ' -f1)

cp "$tmpArchive" "$repoPkg/$checksum.tar.xz"

tmpDb=$(mktemp)

grep -v "^$fullName " "$repoDb" > "$tmpDb"
echo "$fullName $checksum" >> "$tmpDb"

sort "$tmpDb" > "$repoDb"

rm "$tmpArchive"
rm "$tmpDb"
_________________________________________________________________________________________________________



#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 repo package"
    exit 1
fi

repo="$1"
pkg="$2"

if [[ ! -d "$repo" ]]; then
    echo "Repo is not directory"
    exit 1
fi

if [[ ! -f "$repo/db" ]]; then
    echo "Missing repo db"
    exit 1
fi

if [[ ! -d "$repo/packages" ]]; then
    echo "Missing repo packages directory"
    exit 1
fi

if [[ ! -d "$pkg" ]]; then
    echo "Package is not directory"
    exit 1
fi

if [[ ! -f "$pkg/version" ]]; then
    echo "Missing package version"
    exit 1
fi

if [[ ! -d "$pkg/tree" ]]; then
    echo "Missing package tree"
    exit 1
fi

pkg_name=$(basename "$pkg")
pkg_version=$(cat "$pkg/version")
pkg_key="${pkg_name}-${pkg_version}"

tmp_archive=$(mktemp)
tmp_db=$(mktemp)

# правим tar.xz архив само на съдържанието на tree
tar -cJf "$tmp_archive" -C "$pkg/tree" .

checksum=$(sha256sum "$tmp_archive" | cut -d ' ' -f1)

new_archive="$repo/packages/${checksum}.tar.xz"

# ако тази версия вече съществува, намираме стария checksum
old_checksum=$(grep "^${pkg_key} " "$repo/db" | cut -d ' ' -f2)

if [[ -n "$old_checksum" ]]; then
    rm -f "$repo/packages/${old_checksum}.tar.xz"

    grep -v "^${pkg_key} " "$repo/db" > "$tmp_db"
else
    cp "$repo/db" "$tmp_db"
fi

mv "$tmp_archive" "$new_archive"

echo "$pkg_key $checksum" >> "$tmp_db"

sort "$tmp_db" > "$repo/db"

rm -f "$tmp_db"
