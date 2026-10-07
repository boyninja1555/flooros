CC             := gcc
CFLAGS         := -O2 -Wall -Wextra -std=c17 -D_DEFAULT_SOURCE -MMD
QEMU           := qemu-system-x86_64

KERNEL_MAJ     := 7
KERNEL_VER     := 7.1.7
KERNEL_DIR     := linux-$(KERNEL_VER)
KERNEL_TAR     := $(KERNEL_DIR).tar.xz
BZIMAGE        := $(KERNEL_DIR)/arch/x86_64/boot/bzImage

SYS_SRC        := src/sysfloor.c
SHELL_SRC      := src/shellyfloor.c
BUTOOLS_SRC    := $(shell find src/butools/src -name "*.c" 2>/dev/null)

SYS_BIN        := build/init
SHELL_BIN      := build/sf
BUTOOLS_BIN    := $(patsubst src/butools/%.c, build/butools/%, $(BUTOOLS_SRC))
GOONER_BIN     := src/gooner/build
#LIBFLOOR69_BIN := src/libfloor69/build
DEPS           := $(shell find build -name "*.d" 2>/dev/null)

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

$(GOONER_BIN):
	cmake -S src/gooner -B $(GOONER_BIN)
	cmake --build $(GOONER_BIN)

#$(LIBFLOOR69_BIN):
#	cmake -S src/libfloor69 -B $(LIBFLOOR69_BIN)
#	cmake --build $(LIBFLOOR69_BIN)

build/butools/%: src/butools/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

#initramfs.img: $(SYS_BIN) $(SHELL_BIN) $(BUTOOLS_BIN) $(GOONER_BIN) $(LIBFLOOR69_BIN)
initramfs.img: $(SYS_BIN) $(SHELL_BIN) $(BUTOOLS_BIN) $(GOONER_BIN)
	@echo "Packing initramfs..."
	rm -rf rootfs
	mkdir -p rootfs/bin rootfs/bin-h rootfs/dev rootfs/proc rootfs/sys rootfs/lib rootfs/lib/include rootfs/etc rootfs/home
	ln -s lib rootfs/lib64

# "Important" libraries
	cp /lib/x86_64-linux-gnu/libnss_files.so.2 rootfs/lib/
	cp /lib/x86_64-linux-gnu/libnss_dns.so.2 rootfs/lib/

# Subprojects
	cp $(SYS_BIN) rootfs/init
	cp $(SHELL_BIN) rootfs/bin/sf
	cp src/shellyfloor-help rootfs/bin-h/sf
	cp $(BUTOOLS_BIN) rootfs/bin/
	cp src/butools/src-h/* rootfs/bin-h/
	cp $(GOONER_BIN)/gooner rootfs/bin/
	cp src/gooner/help rootfs/bin-h/gooner
#	cp $(LIBFLOOR69_BIN)/libfloor69.a rootfs/lib/
#	cp src/libfloor69/include/libfloor69.h rootfs/lib/include/

# RootFS source
	cp -r rootfs-src/* rootfs/

# Compilablilityness
	cp /bin/as rootfs/bin/
	cp /bin/ld rootfs/bin/
	cp /bin/nasm rootfs/bin/
	cp /lib/x86_64-linux-gnu/libbfd-*-system.so rootfs/lib/
	cp /lib/x86_64-linux-gnu/libz.so.* rootfs/lib/
	cp /lib/x86_64-linux-gnu/libzstd.so.* rootfs/lib/
	cp /lib/x86_64-linux-gnu/libsframe.so.* rootfs/lib/
	cp /lib/x86_64-linux-gnu/libctf.so.* rootfs/lib/
	cp /lib/x86_64-linux-gnu/libjansson.so.* rootfs/lib/

	@echo "Copying dynamic interpreter and shared libraries..."
	@for bin in $(SYS_BIN) $(SHELL_BIN) $(BUTOOLS_BIN); do \
		for lib in $$(ldd $$bin 2>/dev/null | grep -o '/[^\ ]*'); do \
			if [ -f "$$lib" ]; then \
				cp -L "$$lib" rootfs/lib/ 2>/dev/null || true; \
			fi; \
		done; \
	done

	cd rootfs && find . -print0 | cpio --null -ov --format=newc > ../initramfs.img

FloorOS.iso: $(BZIMAGE) initramfs.img
	@echo "Building ISO..."
	mkdir -p iso/boot/grub
	cp initramfs.img iso/boot/initramfs.img
	cp $(BZIMAGE) iso/boot/vmlinuz
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o FloorOS.iso iso

run: $(BZIMAGE) initramfs.img
	$(QEMU) -m 512M -vga std -kernel $(BZIMAGE) -initrd initramfs.img -append "console=tty0"

run-dev: $(BZIMAGE) initramfs.img
	$(QEMU) -m 512M -kernel $(BZIMAGE) -initrd initramfs.img -append "console=ttyS0" -nographic

run-iso: FloorOS.iso
	$(QEMU) -m 512M -cdrom FloorOS.iso -boot d

clean:
	rm -rf rootfs iso build src/gooner/build src/libfloor69/build FloorOS.iso initramfs.img
	@echo "Kernel source was NOT deleted to save time! Delete $(KERNEL_DIR) manually if needed, but generally it'd a bad idea and booooooo."

-include $(DEPS)
