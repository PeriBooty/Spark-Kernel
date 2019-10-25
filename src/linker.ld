KERNEL_LMA = 0x100000;
KERNEL_VMA = 0xffffffff80100000;
ENTRY(entry)

SECTIONS
{
  . = KERNEL_LMA;
  .text :
  {
    KEEP(*(.multiboot))
    *(.text)
    . = ALIGN(4K);
  }

  .data :
  {
    *(.data)
    *(.eh_frame)
    *(.eh_frame_hdr)
    . = ALIGN(4K);
  }

  .bss :
  {
    *(.bss)
    . = ALIGN(4K);
  }

  .rodata : AT (ADDR (.rodata) - KERNEL_VMA)
  {
    *(.rodata)
    . = ALIGN(4K);
  }

  /DISCARD/ : { *(.fini_array*) *(.comment) }
}