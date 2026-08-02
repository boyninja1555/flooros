#!/usr/bin/env bash
set -e
if [ "$PWD" != "$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd )" ]; then
    echo -e "\033[31mYou must run this script from its parent directory!\033[0m"
    exit 1
fi
echo -e "\033[34mBuilding console...\033[0m"
mkdir -p build
cd build
cmake .. -DCMAKE_C_COMPILER=gcc
cmake --build .
cp console ../../rootfs/bin/console
cd ..
echo -e "\033[34mBuilt console to rootfs!\033[0m"