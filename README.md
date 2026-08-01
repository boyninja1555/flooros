To set up your local working directory for FloorOS, we recommend Linux for it's built-in kernel files we can easily copy. Each should be executed sequentially:

```bash
# Initializes root file system directory structure since git won't commit empty directories
./initfs.sh

# Builds root file system to an image used by the kernel
./buildfs.sh

# OPTIONAL: Copies your host machine's kernel for usage with QEMU (qemu.sh)
./copykernel.sh

# OPTIONAL: Runs QEMU using the copied kernel and the rootfs image from above
./qemu.sh
```
