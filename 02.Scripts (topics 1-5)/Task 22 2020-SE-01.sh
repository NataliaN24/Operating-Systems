#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 bar.csv dir"
    exit 1
fi

out="$1"
dir="$2"

# header
echo "hostname,phy,vlans,hosts,failover,VPN-3DES-AES,peers,VLAN Trunk Ports,license,SN,key" > "$out"

find "$dir" -maxdepth 1 -type f -name "*.log" | while read -r file; do

    hostname=$(basename "$file" .log)

    phy=$(grep "Maximum Physical Interfaces" "$file" | tr -s ' ' | cut -d ':' -f2 | tr -d ' ')
    vlans=$(grep "^VLANs" "$file" | tr -s ' ' | cut -d ':' -f2 | tr -d ' ')
    hosts=$(grep "Inside Hosts" "$file" | tr -s ' ' | cut -d ':' -f2 | sed 's/^ *//')
    failover=$(grep "^Failover" "$file" | tr -s ' ' | cut -d ':' -f2 | sed 's/^ *//')
    vpn=$(grep "VPN-3DES-AES" "$file" | tr -s ' ' | cut -d ':' -f2 | sed 's/^ *//')

    peers=$(grep "Total VPN Peers" "$file" | tr -s ' ' | cut -d ':' -f2 | tr -d ' ')
    trunk=$(grep "VLAN Trunk Ports" "$file" | tr -s ' ' | cut -d ':' -f2 | tr -d ' ')

    license=$(grep "This platform has" "$file" | cut -d ' ' -f5- | sed 's/ licence\.//')
    sn=$(grep "Serial Number" "$file" | tr -s ' ' | cut -d ':' -f2 | tr -d ' ')
    key=$(grep "Running Activation Key" "$file" | tr -s ' ' | cut -d ':' -f2 | tr -d ' ')

    echo "$hostname,$phy,$vlans,$hosts,$failover,$vpn,$peers,$trunk,$license,$sn,$key" >> "$out"

done
