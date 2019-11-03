# Spark Kernel
The kernel for the Spark Operating System.
### Compiling the kernel
#### Compiling the kernel - the dependencies
* binutils*
* gcc*
* make
* nasm
* xorriso
* grub-common
* grub-pc-bin
# Cross-Compiler
***You need a x86_64-elf cross-compiler to build this kernel, here's a guide on [how to get one](https://wiki.osdev.org/GCC_Cross-Compiler)**

Run `make` where the Makefile is and an ISO will automagically be made for you inside the `out` directory.