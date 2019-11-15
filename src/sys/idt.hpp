#pragma once

/// Interrupt Descriptor Table entry struct
struct [[gnu::packed]] IdtEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t attrib;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
};

/// Interrupt Descriptor Table pointer
struct [[gnu::packed]] IdtPtr {
    uint16_t limit;
    uint64_t base;
};

/// Initializes the Interrupt Descriptor Table
void idt_init();

/// IRQ 0 handler from boot.asm
extern "C" void irq0();

/// IRQ 1 handler from boot.asm
extern "C" void irq1();