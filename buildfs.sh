#!/usr/bin/env bash
set -e
if [ "$PWD" != "$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd )" ]; then
    echo -e "\033[31mYou must run this script from its parent directory!\033[0m"
    exit 1
fi
SOURCE=rootfs/
TARGET=initramfs.img
echo -e "\033[34mBuilding $SOURCE...\033[0m"
if [ ! -f "$SOURCE/init" ]; then
    cd sysfloor
    ./build.sh
    cd ..
fi
find rootfs -print | cpio -ov -H newc > initramfs.img
echo -e "\033[34mBuilt $SOURCE to $TARGET!\033[0m"