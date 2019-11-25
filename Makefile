GPP_PARAMS = -Ispark -m64 -ffreestanding -fno-use-cxa-atexit -fno-pic \
	-mno-sse -mno-sse2 -fno-builtin -fno-rtti -fno-exceptions \
	-fno-leading-underscore -fno-stack-protector -mno-red-zone -mcmodel=kernel -std=c++17 \
	-Wall -Wextra -Werror -fsigned-char
NASM_PARAMS = -felf64 -F dwarf
LINKER_PARAMS = -melf_x86_64 -no-pie -nostdlib
OBJECTS = $(patsubst spark/%.cpp, out/%.o, $(shell find spark -name *.cpp))
OBJECTS += $(patsubst spark/%.asm, out/%.o, $(shell find spark -name *.asm))
TARGET = release

ifeq ($(TARGET), debug)
	GPP_PARAMS += -Og -g
	NASM_PARAMS += -g
	LINKER_PARAMS += -Og
else
	GPP_PARAMS += -O3
	LINKER_PARAMS += -O3
endif

.PHONY: all mkdirs bin iso clean

all: mkdirs iso

mkdirs:
	mkdir -p out/hardware/devices
	mkdir -p out/hardware/mm
	mkdir -p out/hardware/cpu/smp
	mkdir -p out/lib
	mkdir -p out/hardware/acpi
	mkdir -p out/iso/system

out/%.o: spark/%.cpp
	x86_64-elf-g++ $(GPP_PARAMS) -o $@ -c $<

out/%.o: spark/%.asm
	nasm $< $(NASM_PARAMS) -o $@

bin: $(OBJECTS)
	x86_64-elf-ld $(LINKER_PARAMS) -T spark/linker.ld -o out/iso/system/kernel.bin $^

iso: bin
	mkdir -p out/iso/boot/grub
	cp grub.cfg out/iso/boot/grub/grub.cfg
	grub-mkrescue --output=out/spark.iso out/iso

clean:
	rm -rf out