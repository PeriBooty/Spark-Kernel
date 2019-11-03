GCCPARAMS = -Isrc -m64 -ffreestanding -fno-use-cxa-atexit -fno-pic \
	-mno-sse -mno-sse2 -fno-builtin -fno-rtti -fno-exceptions \
	-fno-leading-underscore -fno-stack-protector -std=c++17 -Og -mno-red-zone \
	-mcmodel=kernel -g
CXXFLAGS = -Wall -Wextra -Werror
NASMPARAMS = -felf64 -F dwarf -g
LDPARAMS = -melf_x86_64 -no-pie -nostdlib -Og
OBJECTS = out/boot.o out/kernel.o out/hardware/port.o \
	out/hardware/devices/vbe.o out/lib/lib.o out/hardware/cpu/lock.o \
	out/hardware/mm/mm.o out/hardware/mm/pmm.o out/hardware/mm/vmm.o \
	out/lib/spinlock.o out/hardware/cpu/cpu.o out/sys/idt.o \
	out/sys/panic.o

all: clean pre iso

pre:
	mkdir -p out/hardware/devices
	mkdir -p out/hardware/mm
	mkdir -p out/hardware/cpu
	mkdir -p out/lib
	mkdir -p out/sys

out/%.o: src/%.cpp
	x86_64-elf-g++ $(GCCPARAMS) $(CXXFLAGS) -o $@ -c $<

out/%.o: src/%.asm
	nasm $< $(NASMPARAMS) -o $@

Kernel.bin: src/linker.ld $(OBJECTS)
	ld $(LDPARAMS) -T $< -o out/$@ $(OBJECTS)

iso: Kernel.bin
	mkdir -p out/iso/boot/grub
	mv out/Kernel.bin out/iso/boot/Kernel.bin
	cp grub.cfg out/iso/boot/grub/grub.cfg
	grub-mkrescue --output=out/SparkOS.iso out/iso

clean:
	rm -rf out