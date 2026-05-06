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
