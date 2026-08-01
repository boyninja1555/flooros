#!/usr/bin/env bash
set -e
if [ "$PWD" != "$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd )" ]; then
    echo -e "\033[31mYou must run this script from its parent directory!\033[0m"
    exit 1
fi
echo -e "\033[34mBuilding shellyfloor...\033[0m"
gcc -static shellyfloor.c -o ../rootfs/bin/sf
echo -e "\033[34mBuilt shellyfloor to rootfs!\033[0m"