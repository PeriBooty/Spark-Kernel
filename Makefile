CLANGPARAMS := -Ispark -target x86_64-unknown-elf -ffreestanding -fno-use-cxa-atexit -fno-pic \
	-nostdlib -nostdinc -fno-pie -mno-sse -mno-sse2 \
	-fno-builtin -fno-rtti -fno-exceptions -fsigned-char -fno-stack-protector \
	-mno-red-zone -mcmodel=kernel -std=c++17 -Wall -Wextra -Werror \
	-static -g -DSTB_SPRINTF_NOFLOAT -m64
NASMPARAMS := -felf64 -F dwarf -g
LDPARAMS := -melf_x86_64 --nostdlib --Bstatic -T spark/linker.ld
OBJECTS := $(patsubst spark/%.cpp, out/%.o, $(shell find spark -name *.cpp))
OBJECTS += $(patsubst spark/%.asm, out/%.o, $(shell find spark -name *.asm))
TARGET := DEBUG

ifeq ($(TARGET), RELEASE)
	CLANGPARAMS += -O3
	LDPARAMS += -O3
endif

.PHONY: all mkdirs bin iso clean

all: clean mkdirs iso

mkdirs:
	mkdir -p out/hardware/devices
	mkdir -p out/hardware/mm
	mkdir -p out/hardware/cpu/smp
	mkdir -p out/lib
	mkdir -p out/hardware/acpi
	mkdir -p out/iso/system

out/%.o: spark/%.cpp
	clang++ $(CLANGPARAMS) -o $@ -c $<

out/%.o: spark/%.asm
	nasm $< $(NASMPARAMS) -o $@

bin: $(OBJECTS)
	ld.lld $(LDPARAMS) -o out/iso/system/kernel.bin $^

iso: bin
	mkdir -p out/iso/boot/grub
	cp grub.cfg out/iso/boot/grub/grub.cfg
	grub-mkrescue --output=out/spark.iso out/iso

clean:
	rm -rf out