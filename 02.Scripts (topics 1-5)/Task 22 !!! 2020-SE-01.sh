#!/bin/bash

if [[ $# -ne 2 ]]; then
  echo "Usage: $0 bar.csv <dir>" >&2
  exit 1
fi

out="$1"
dir="$2"

if [[ ! -d "$dir" ]]; then
  echo "Not a directory: $dir" >&2
  exit 2
fi

# helper: взима стойността след ":" за първия ред, който мачва pattern-а
# и маха водещи/следващи интервали
get_val() {
  local file="$1"
  local pat="$2"
  grep -m 1 -E "$pat" "$file" \
    | cut -d ':' -f2- \
    | sed -E 's/^ +//; s/ +$//'
}

# helper за license реда: "This platform has a XXX license."
get_license() {
  local file="$1"
  grep -m 1 -E '^This platform has a ' "$file" \
    | sed -E 's/^This platform has a //; s/ license\.$//; s/^ +//; s/ +$//'
}

# header
echo "hostname,phy,vlans,hosts,failover,VPN-3DES-AES,peers,VLAN Trunk Ports,license,SN,key" > "$out"

# обхождаме всички .log файлове в директорията
# (ако няма, цикълът ще вземе буквално '*.log' -> затова проверяваме -f)
for f in "$dir"/*.log; do
  [[ -f "$f" ]] || continue

  base="$(basename "$f")"
  host="${base%.log}"

  phy="$(get_val "$f" '^Maximum Physical Interfaces[ ]*:')"
  vlans="$(get_val "$f" '^VLANs[ ]*:')"
  hosts="$(get_val "$f" '^Inside Hosts[ ]*:')"
  failover="$(get_val "$f" '^Failover[ ]*:')"
  vpn="$(get_val "$f" '^VPN-3DES-AES[ ]*:')"
  peers="$(get_val "$f" '^\*Total VPN Peers[ ]*:')"
  trunk="$(get_val "$f" '^VLAN Trunk Ports[ ]*:')"
  lic="$(get_license "$f")"
  sn="$(get_val "$f" '^Serial Number[ ]*:')"
  key="$(get_val "$f" '^Running Activation Key[ ]*:')"

  echo "${host},${phy},${vlans},${hosts},${failover},${vpn},${peers},${trunk},${lic},${sn},${key}" >> "$out"
done
