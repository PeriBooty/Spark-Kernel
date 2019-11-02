GPP_PARAMS = -Isrc -m64 -ffreestanding -fno-use-cxa-atexit -fno-pic \
	-mno-sse -mno-sse2 -fno-builtin -fno-rtti -fno-exceptions \
	-fno-leading-underscore -fno-stack-protector -std=c++17 -Og -mno-red-zone \
	-mcmodel=kernel -g -Wall -Wextra -Werror
NASM_PARAMS = -felf64 -F dwarf -g
LD_PARAMS = -melf_x86_64 -no-pie -nostdlib -Og
CPP_SOURCES = ${shell find src -name *.cpp}
ASM_SOURCES = ${shell find src -name *.asm}
CPP_OBJECTS = ${CPP_SOURCES:.d=.o}
ASM_OBJECTS = ${ASM_SOURCES:.asm=.o}

all: clean mkdirs iso

mkdirs:
	mkdir -p out/iso/boot

%.o: %.cpp
	x86_64-elf-gcc ${GPP_PARAMS} -o out/$@ -c $<

%.o: %.asm
	nasm $< ${NASM_PARAMS} -o out/$@

bin: ${CPP_OBJECTS} ${ASM_OBJECTS}
	cd out/ && ld ${LD_PARAMS} -T ../src/linker.ld  -o iso/boot/kernel.bin ${CPP_OBJECTS} ${ASM_OBJECTS}

iso: bin
	mkdir -p out/iso/boot/grub
	cp grub.cfg out/iso/boot/grub/grub.cfg
	grub-mkrescue --output=out/SparkOS.iso out/iso

clean:
	rm -rf out