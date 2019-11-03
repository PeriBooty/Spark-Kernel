GPP_PARAMS = -Isrc -m64 -ffreestanding -fno-use-cxa-atexit -fno-pic \
	-mno-sse -mno-sse2 -fno-builtin -fno-rtti -fno-exceptions \
	-fno-leading-underscore -fno-stack-protector -std=c++17 -Og -mno-red-zone \
	-mcmodel=kernel -g
CXX_FLAGS = -Wall -Wextra -Werror
NASM_PARAMS = -felf64 -F dwarf -g
LINKER_PARAMS = -melf_x86_64  -no-pie -nostdlib -Og
OBJECTS = out/boot.o out/kernel.o out/hardware/port.o \
	out/hardware/devices/vbe.o out/lib/lib.o out/hardware/cpu/lock.o \
	out/hardware/mm/mm.o out/hardware/mm/pmm.o out/hardware/mm/vmm.o \
	out/lib/spinlock.o out/hardware/cpu/cpu.o out/sys/idt.o \
	out/sys/panic.o

all: clean mkdirs iso

mkdirs:
	mkdir -p out/hardware/devices
	mkdir -p out/hardware/mm
	mkdir -p out/hardware/cpu
	mkdir -p out/lib
	mkdir -p out/sys
	mkdir -p out/iso/system

out/%.o: src/%.cpp
	x86_64-elf-g++ $(GPP_PARAMS) $(CXX_FLAGS) -o $@ -c $<

out/%.o: src/%.asm
	nasm $< $(NASM_PARAMS) -o $@

bin: $(OBJECTS)
	x86_64-elf-ld $(LINKER_PARAMS) -T src/linker.ld -o out/iso/system/kernel.bin $^

iso: bin
	mkdir -p out/iso/boot/grub
	cp grub.cfg out/iso/boot/grub/grub.cfg
	grub-mkrescue --output=out/spark.iso out/iso

clean:
	rm -rf out