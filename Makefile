GCCPARAMS = -Isrc -m64 -ffreestanding -fno-use-cxa-atexit -fno-pic \
	-mno-sse -mno-sse2 -fno-builtin -fno-rtti \
	-fno-exceptions -fno-leading-underscore -fno-stack-protector -std=c++17 -Ofast \
	-fsanitize=undefined -mno-red-zone -mcmodel=kernel
CXXFLAGS = -Wall -Wextra
NASMPARAMS = -f elf64
LDPARAMS = -melf_x86_64 -no-pie -nostdlib -Ofast
OBJECTS = out/x86_64/boot.o out/x86_64/kernel.o out/x86_64/hardware/cpu.o out/x86_64/hardware/port.o out/x86_64/hardware/devices/display.o out/x86_64/lib/lib.o

all: clean pre iso

pre:
	mkdir -p out/x86_64/hardware/devices
	mkdir -p out/x86_64/lib

out/x86_64/%.o: src/%.cpp
	x86_64-elf-g++ $(GCCPARAMS) $(CXXFLAGS) -o $@ -c $<

out/x86_64/hardware/%.o: src/hardware/%.cpp
	x86_64-elf-g++ $(GCCPARAMS) $(CXXFLAGS) -o $@ -c $<

out/x86_64/hardware/devices/%.o: src/hardware/devices/%.cpp
	x86_64-elf-g++ $(GCCPARAMS) $(CXXFLAGS) -o $@ -c $<

out/x86_64/%.o: src/%.S
	nasm $< $(NASMPARAMS) -o $@

Kernel.bin: src/linker.ld $(OBJECTS)
	ld $(LDPARAMS) -T $< -o out/x86_64/$@ $(OBJECTS)

iso: Kernel.bin
	mkdir -p out/x86_64/iso/boot/grub
	mv out/x86_64/Kernel.bin out/x86_64/iso/boot/Kernel.bin
	echo 'set timeout=0' >> out/x86_64/iso/boot/grub/grub.cfg
	echo 'set default=0' >> out/x86_64/iso/boot/grub/grub.cfg
	echo '' >> out/x86_64/iso/boot/grub/grub.cfg
	echo 'menuentry "Spark" {' >> out/x86_64/iso/boot/grub/grub.cfg
	echo '	multiboot /boot/Kernel.bin' >> out/x86_64/iso/boot/grub/grub.cfg
	echo '	boot' >> out/x86_64/iso/boot/grub/grub.cfg
	echo '}' >> out/x86_64/iso/boot/grub/grub.cfg
	grub-mkrescue --output=out/x86_64/SparkOS.iso out/x86_64/iso

clean:
	rm -rf out