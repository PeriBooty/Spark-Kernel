#pragma once

namespace Spark::Idt {
    /// Interrupt Descriptor Table entry struct
    struct [[gnu::packed]] Entry {
        uint16_t offset_low;
        uint16_t selector;
        uint8_t ist;
        uint8_t attrib;
        uint16_t offset_mid;
        uint32_t offset_high;
        uint32_t zero;
    };

    /// Interrupt Descriptor Table pointer
    struct [[gnu::packed]] Pointer {
        uint16_t limit;
        uint64_t base;
    };

    /// Initializes the Interrupt Descriptor Table
    void init();

    /// Adds a callback for an interrupt
    void set_gate(uint8_t vec, uintptr_t function, uint16_t selector, uint8_t flags);

    /// Sends to the PIC master and/or the slave an << END OF INTERRUPT >>
    void irq_eoi(uint8_t irq);

    // Masks/Disables an IRQ
    void mask_irq(uint8_t irq);

    // Unmasks/Enables an IRQ
    void unmask_irq(uint8_t irq);

    /// Changes the PIT's frequency
    void pit_set_frequency(uint32_t frequency);
};  // namespace Spark::Idt