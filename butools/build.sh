#!/usr/bin/env bash
set -e
if [ "$PWD" != "$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd )" ]; then
    echo -e "\033[31mYou must run this script from its parent directory!\033[0m"
    exit 1
fi
echo -e "\033[34mBuilding butools...\033[0m"
for filepath in src/*.c; do
    [ -f "$filepath" ] || continue
    filename=$(basename "$filepath")
    basename="${filepath##*/}"
    basename="${basename%.c}"
    gcc -static src/$basename.c -o ../rootfs/bin/$basename
    echo "Built $basename.c to rootfs!"
done
echo -e "\033[34mFinished building butools to rootfs!\033[0m"