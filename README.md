# Spark Kernel
The kernel for the Spark Operating System.

## Features
- VBE Framebuffer graphics with lean-and-mean text drawing
- Bitmap physical memory management
- 4-level paging
- Interrupts
- Panic
- Terminal
- Symmetric Multiprocessing (SMP)

## Planned features
- ACPI (currently being worked on)
- (U)EFI
- VFS
- R/W from disk
- FS
- Drivers
- API
- Userspace
- Multitasking


(this list is not sorted by priority)

## Compiling the kernel
### Compiling the kernel - the dependencies
- binutils
- CLang
- make
- nasm
- xorriso *or similar*
- grub-common *or similar*
- grub-pc-bin *or similar*

Run `make` where the Makefile is and an ISO will automagically be made for you inside the `out` directory.

For debug builds run `make TARGET=DEBUG`, and it will optimize the code for debugging experience (output directory doesn't change).