GCCPARAMS = -Isrc -m64 -ffreestanding -fno-use-cxa-atexit -fno-pic \
	-mno-sse -mno-sse2 -fno-builtin -fno-rtti \
	-fno-exceptions -fno-leading-underscore -fno-stack-protector -std=c++17 -Og \
	-fsanitize=undefined -mno-red-zone -mcmodel=kernel -g
CXXFLAGS = -Wall -Wextra
NASMPARAMS = -felf64 -F dwarf -g
LDPARAMS = -melf_x86_64 -no-pie -nostdlib -Og
OBJECTS = out/x86_64/boot.o out/x86_64/kernel.o out/x86_64/version.o \
	out/x86_64/hardware/port.o out/x86_64/hardware/devices/vbe.o out/x86_64/lib/lib.o \
	out/x86_64/hardware/lock.o out/x86_64/hardware/mm/mm.o out/x86_64/hardware/mm/pmm.o \
	out/x86_64/hardware/mm/vmm.o out/x86_64/lib/spinlock.o

all: clean pre iso

pre:
	mkdir -p out/x86_64/hardware/devices
	mkdir -p out/x86_64/hardware/mm
	mkdir -p out/x86_64/lib

out/x86_64/%.o: src/%.cpp
	x86_64-elf-g++ $(GCCPARAMS) $(CXXFLAGS) -o $@ -c $<

out/x86_64/hardware/%.o: src/hardware/%.cpp
	x86_64-elf-g++ $(GCCPARAMS) $(CXXFLAGS) -o $@ -c $<

out/x86_64/hardware/devices/%.o: src/hardware/devices/%.cpp
	x86_64-elf-g++ $(GCCPARAMS) $(CXXFLAGS) -o $@ -c $<

out/x86_64/hardware/mm/%.o: src/hardware/mm/%.cpp
	x86_64-elf-g++ $(GCCPARAMS) $(CXXFLAGS) -o $@ -c $<

out/x86_64/%.o: src/%.asm
	nasm $< $(NASMPARAMS) -o $@

Kernel.bin: src/linker.ld $(OBJECTS)
	ld $(LDPARAMS) -T $< -o out/x86_64/$@ $(OBJECTS)

iso: Kernel.bin
	mkdir -p out/x86_64/iso/boot/grub
	mv out/x86_64/Kernel.bin out/x86_64/iso/boot/Kernel.bin
	cp grub.cfg out/x86_64/iso/boot/grub/grub.cfg
	grub-mkrescue --output=out/x86_64/SparkOS.iso out/x86_64/iso

clean:
	rm -rf out