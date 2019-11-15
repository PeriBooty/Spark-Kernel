GPP_PARAMS = -Isrc -m64 -ffreestanding -fno-use-cxa-atexit -fno-pic \
	-mno-sse -mno-sse2 -fno-builtin -fno-rtti -fno-exceptions \
	-fno-leading-underscore -fno-stack-protector -mno-red-zone -mcmodel=kernel -std=c++17\
	-Wall -Wextra -Werror
NASM_PARAMS = -felf64 -F dwarf
LINKER_PARAMS = -melf_x86_64 -no-pie -nostdlib
OBJECTS = $(patsubst src/%.cpp, out/%.o, $(shell find src -name *.cpp))
OBJECTS += $(patsubst src/%.asm, out/%.o, $(shell find src -name *.asm))
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

all: clean mkdirs iso

mkdirs:
	mkdir -p out/hardware/devices
	mkdir -p out/hardware/mm
	mkdir -p out/hardware/cpu
	mkdir -p out/lib
	mkdir -p out/sys
	mkdir -p out/iso/system

out/%.o: src/%.cpp
	x86_64-elf-g++ $(GPP_PARAMS) -o $@ -c $<

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