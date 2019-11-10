#pragma once

struct [[gnu::packed]] IdtEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t attrib;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
};

struct [[gnu::packed]] IdtPtr {
    uint16_t limit;
    uint64_t base;
};

void idt_init();
void idt_set_gate(uint8_t vec, uintptr_t function, uint16_t selector, uint8_t flags);
void irq_eoi(uint8_t irq);
void mask_irq(uint8_t irq);
void unmask_irq(uint8_t irq);
void init_pit(uint32_t frequency);

extern "C" void irq0();
extern "C" void irq1();