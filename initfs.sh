#!/usr/bin/env bash
set -e
SOURCE=rootfs/
if [ "$PWD" != "$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd )" ]; then
    echo -e "\033[31mYou must run this script from its parent directory!\033[0m"
    exit 1
fi
if [ ! -d "$SOURCE" ]; then
    mkdir -p $SOURCE/{dev,proc,sys,tmp}
    echo -e "\033[34mInitialized $SOURCE!\033[0m"
fi