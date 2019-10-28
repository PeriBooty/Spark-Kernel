ENTRY(_loader)
KERNEL_VMA = 0xFFFFFFFF80000000;

SECTIONS
{
  . = KERNEL_VMA + 0x1000000;
  
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

  /DISCARD/ : { *(.fini_array*) *(.comment) }
}