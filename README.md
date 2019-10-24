![forthebadge](https://forthebadge.com/images/badges/built-with-love.svg) ![forthebadge](https://forthebadge.com/images/badges/made-with-c-plus-plus.svg)

# Spark Kernel
The kernel.
### Compiling the kernel
#### Compiling the kernel - the dependencies
* binutils*
* g++*
* make
* nasm
* xorriso
* grub-common
* grub-pc-bin
# Cross-Compiler
***You need a cross-compiler to build this kernel, here's a [guide on how to get one](https://wiki.osdev.org/GCC_Cross-Compiler)**


#### Compiling the kernel on Windows
If you would like to build this project on Windows, you can use Windows Subsystem for Linux or similar.


#### Now we are actually compiling
Run `make` where the Makefile is and an ISO will magically be made for you inside `out/x86/`.
