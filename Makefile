CC          := gcc
CFLAGS      := -static -O2 -Wall -Wextra -std=c17 -D_DEFAULT_SOURCE -MMD
QEMU        := qemu-system-x86_64

KERNEL_MAJ  := 7
KERNEL_VER  := 7.1.7
KERNEL_DIR  := linux-$(KERNEL_VER)
KERNEL_TAR  := $(KERNEL_DIR).tar.xz
BZIMAGE     := $(KERNEL_DIR)/arch/x86_64/boot/bzImage

SYS_SRC     := $(shell find sysfloor -name "*.c" 2>/dev/null)
SHELL_SRC   := $(shell find shellyfloor -name "*.c" 2>/dev/null)
BUTOOLS_SRC := $(shell find butools -name "*.c" 2>/dev/null)

SYS_BIN     := build/init
SHELL_BIN   := build/sf
BUTOOLS_BIN := $(patsubst butools/%.c, build/butools/%, $(BUTOOLS_SRC))
DEPS        := $(shell find build -name "*.d" 2>/dev/null)

.PHONY: all clean run run-dev run-iso
all: FloorOS.iso

$(BZIMAGE): kernel.config
	@if [ ! -d "$(KERNEL_DIR)" ]; then \
		echo "Downloading Linux $(KERNEL_VER)..."; \
		wget -c "https://cdn.kernel.org/pub/linux/kernel/v$(KERNEL_MAJ).x/$(KERNEL_TAR)"; \
		tar -xf "$(KERNEL_TAR)"; \
	fi
	cp kernel.config $(KERNEL_DIR)/.config
	$(MAKE) -C $(KERNEL_DIR) olddefconfig
	$(MAKE) -C $(KERNEL_DIR) -j$$(nproc) bzImage

$(SYS_BIN): $(SYS_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

$(SHELL_BIN): $(SHELL_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

build/butools/%: butools/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

initramfs.img: $(SYS_BIN) $(SHELL_BIN) $(BUTOOLS_BIN)
	@echo "Packing initramfs..."
	rm -rf rootfs
	mkdir -p rootfs/bin rootfs/dev rootfs/proc rootfs/sys
	cp $(SYS_BIN) rootfs/init
	cp $(SHELL_BIN) rootfs/bin/sf
	cp $(BUTOOLS_BIN) rootfs/bin/
	cd rootfs && find . -print0 | cpio --null -ov --format=newc > ../initramfs.img

FloorOS.iso: $(BZIMAGE) initramfs.img
	@echo "Building ISO..."
	mkdir -p iso/boot/grub
	cp initramfs.img iso/boot/initramfs.img
	cp $(BZIMAGE) iso/boot/vmlinuz
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "FloorOS" {' >> iso/boot/grub/grub.cfg
	echo '    linux /boot/vmlinuz console=tty0' >> iso/boot/grub/grub.cfg
	echo '    initrd /boot/initramfs.img' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue -o FloorOS.iso iso

run: $(BZIMAGE) initramfs.img
	$(QEMU) -m 512M -kernel $(BZIMAGE) -initrd initramfs.img -append "console=tty0"

run-dev: $(BZIMAGE) initramfs.img
	$(QEMU) -m 512M -kernel $(BZIMAGE) -initrd initramfs.img -append "console=ttyS0" -nographic

run-iso: FloorOS.iso
	$(QEMU) -m 512M -cdrom FloorOS.iso -boot d

clean:
	rm -rf rootfs iso build FloorOS.iso initramfs.img
	@echo "Kernel source was NOT deleted to save time! Delete $(KERNEL_DIR) manually if needed, but generally it'd a bad idea and booooooo."

-include $(DEPS)
