#!/bin/bash

# 1) Проверки
if [[ $# -ne 2 ]]; then
  echo "Usage: $0 <repo_path> <package_path>" >&2
  exit 1
fi

repo="$1"
pkgdir="$2"

db="$repo/db"
pkgstore="$repo/packages"

if [[ ! -d "$repo" || ! -f "$db" || ! -d "$pkgstore" ]]; then
  echo "Error: invalid repo (need: repo/db and repo/packages/)" >&2
  exit 1
fi

if [[ ! -d "$pkgdir" || ! -f "$pkgdir/version" || ! -d "$pkgdir/tree" ]]; then
  echo "Error: invalid package (need: <pkg>/version and <pkg>/tree/)" >&2
  exit 1
fi

pkgname="$(basename "$pkgdir")"
pkgver="$(cat "$pkgdir/version")"

# махаме trailing/leading whitespace (ако случайно има)
pkgver="$(echo "$pkgver" | tr -d '\r' | sed 's/^[[:space:]]*//; s/[[:space:]]*$//')"

if [[ -z "$pkgver" ]]; then
  echo "Error: empty version file" >&2
  exit 1
fi

key="${pkgname}-${pkgver}"

# 2) Правим архив на tree/ в tmp файл
tmp_archive="$(mktemp)"
tmp_db="$(mktemp)"

# tar съдържанието на tree (само вътрешността), после го компресираме с xz
# Получава се .tar.xz
tar -C "$pkgdir/tree" -cf - . | xz -z -c > "$tmp_archive"

# 3) sha256 checksum на архива
newsum="$(sha256sum "$tmp_archive" | cut -d ' ' -f1)"

# 4) Проверяваме дали key вече съществува в db
# Точно съвпадение на първа колона:
oldsum="$(awk -v k="$key" '$1==k {print $2}' "$db" | head -n 1 || true)"

if [[ -n "$oldsum" ]]; then
  # 4a) Ако съществува: махаме стария архив (ако го има)
  oldfile="$pkgstore/${oldsum}.tar.xz"
  if [[ -f "$oldfile" ]]; then
    rm -f "$oldfile"
  fi

  # 4b) Подменяме реда в db: key -> newsum
  awk -v k="$key" -v s="$newsum" '
    BEGIN{done=0}
    $1==k {print k, s; done=1; next}
    {print}
    END{
      if(done==0){ print k, s }
    }
  ' "$db" > "$tmp_db"
else
  # 5) Ако не съществува: добавяме нов ред
  cat "$db" > "$tmp_db"
  echo "$key $newsum" >> "$tmp_db"
fi

# 6) Сортираме db лексикографски и го заменяме атомично
sort "$tmp_db" > "${tmp_db}.sorted"
mv "${tmp_db}.sorted" "$db"

# 7) Преместваме архива в repo/packages под името checksum.tar.xz
mv "$tmp_archive" "$pkgstore/${newsum}.tar.xz"

# 8) чистене на tmp (ако остане)
rm -f "$tmp_db"
