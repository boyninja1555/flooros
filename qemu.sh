#!/usr/bin/env bash
set -e
if [ "$PWD" != "$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd )" ]; then
    echo -e "\033[31mYou must run this script from its parent directory!\033[0m"
    exit 1
fi
./buildfs.sh
qemu-system-x86_64 -kernel ./vmlinuz-$(uname -r) -initrd initramfs.img -append "console=ttyS0" -nographic