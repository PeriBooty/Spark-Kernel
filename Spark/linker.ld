ENTRY(loader)
KERNEL_VMA = 0xFFFFFFFF80000000;

SECTIONS {
	. = KERNEL_VMA + 0x200000;

	.text : AT(ADDR(.text) - KERNEL_VMA) {
		*(.multiboot)
		*(.text)
		_rodata = .;
		*(.rodata*)
	}

	.data ALIGN (0x1000) : AT(ADDR(.data) - KERNEL_VMA) {
		*(.data)
	}

	.rodata ALIGN (4096) : AT (ADDR (.rodata) - KERNEL_VMA) {
		*(.rodata*)
		_init_array_start = .;
		KEEP(*(SORT(.ctors)))
		KEEP(*(.ctors))
		_init_array_end = .;
	}

	.bss : AT(ADDR(.bss) - KERNEL_VMA) {
		_sbss = .;
		*(COMMON)
		*(.bss)
		_ebss = .;
	}

	. = 0x1000;

	.trampoline : AT(0x400000) {
		_trampoline_start = .;
		*(.trampoline)
		_trampoline_end = .;
   }
}