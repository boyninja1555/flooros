#!/usr/bin/env bash
set -e
if [ "$PWD" != "$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd )" ]; then
    echo -e "\033[31mYou must run this script from its parent directory!\033[0m"
    exit 1
fi
SOURCE=/boot/vmlinuz-$(uname -r)
if [ ! -f "$SOURCE" ]; then
    echo -e "\033[31mLinux kernel not found at $SOURCE!\033[0m"
    exit 1
fi
TARGET=vmlinuz-$(uname -r)
echo -e "\033[34mCopying Linux kernel from your host machine...\033[0m"
echo -e "Version is $(uname -r)"
sudo cp $SOURCE $TARGET
sudo chown $USER:$USER $TARGET
echo -e "\033[34mCopied kernel!\033[0m"