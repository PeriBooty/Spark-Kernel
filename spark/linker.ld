ENTRY(loader)
KERNEL_LMA = 0x0000000000200000;
KERNEL_SMP = 0x0000000000400000;
KERNEL_VMA = 0xFFFFFFFF80000000;

SECTIONS {
	. = KERNEL_VMA + KERNEL_LMA;

	.text : AT(ADDR(.text) - KERNEL_VMA) {
		*(.multiboot)
		*(.text)
		_rodata = .;
		*(.rodata*)
	}

	.data ALIGN (0x1000) : AT(ADDR(.data) - KERNEL_VMA) {
		*(.data)
	}

	.bss : AT(ADDR(.bss) - KERNEL_VMA) {
		_sbss = .;
		*(COMMON)
		*(.bss)
		_ebss = .;
	}

	. = 0x1000;

	.trampoline : AT(KERNEL_SMP) {
		_trampoline_start = .;
		*(.trampoline)
		_trampoline_end = .;
   }
}