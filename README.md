# Spark Kernel
The kernel for the Spark Operating System.

## Features
- VBE Framebuffer graphics with lean-and-mean text drawing
- Bitmap physical memory management
- 4-level paging
- Interrupts
- Panic
- Terminal

## Planned features
- ACPI (currently being worked on)
- (U)EFI
- VFS
- R/W from disk
- FS
- Drivers
- API
- Userspace


(this list is not sorted by priority)

## Compiling the kernel
### Compiling the kernel - the dependencies
- binutils*
- gcc*
- make
- nasm
- xorriso
- grub-common
- grub-pc-bin

***You need a x86_64-elf cross-compiler to build this kernel, here's a guide on [how to get one](https://wiki.osdev.org/GCC_Cross-Compiler)**

Run `make` where the Makefile is and an ISO will automagically be made for you inside the `out` directory.

For debug builds run `make TARGET=debug`, and it will optimize the code for debugging experience (output directory doesn't change).