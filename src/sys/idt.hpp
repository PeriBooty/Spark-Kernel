#pragma once

/// Interrupt Descriptor Table entry struct
struct [[gnu::packed]] InterruptDescriptorTableEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t attrib;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
};

/// Interrupt Descriptor Table pointer
struct [[gnu::packed]] InterruptDescriptorTablePointer {
    uint16_t limit;
    uint64_t base;
};

class InterruptDescriptorTable {
private:
    static InterruptDescriptorTableEntry idt_entries[256];  // Interrupt Descriptor Table
    static InterruptDescriptorTablePointer idt_pointer;     // Interrupt Descriptor Table pointer
    static int ticks;                  // PIT total ticks
public:
    /// Initializes the Interrupt Descriptor Table
    static void init();

    /// Adds a callback for an interrupt
    static void set_gate(uint8_t vec, uintptr_t function, uint16_t selector, uint8_t flags);

    /// Sends to the PIC master and/or the slave an << END OF INTERRUPT >>
    static void irq_eoi(uint8_t irq);

    // Masks/Disables an IRQ
    static void mask_irq(uint8_t irq);

    // Unmasks/Enables an IRQ
    static void unmask_irq(uint8_t irq);

    /// Changes the PIT's frequency
    static void pit_set_frequency(uint32_t frequency);
};

/// IRQ 0 handler from boot.asm
extern "C" void irq0();

/// IRQ 1 handler from boot.asm
extern "C" void irq1();