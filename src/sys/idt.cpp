#include <sys/idt.hpp>
#include <hardware/port.hpp>

IdtEntry idt[256];
IdtPtr idt_ptr;

void idt_init() {
    idt_set_gate(9, (uintptr_t)irq1, 0x08, 0x8E);
    idt_ptr.limit = 256 * sizeof(IdtEntry) - 1;
    idt_ptr.base = (uint64_t)&idt;
    asm volatile("sti");
    asm volatile("lidt %0" ::"m"(idt_ptr));
}

void idt_set_gate(uint8_t vec, uintptr_t function, uint16_t selector, uint8_t flags) {
    idt[vec].offset_low = function & 0xFFFF;
    idt[vec].offset_mid = (function >> 16) & 0xFFFF;
    idt[vec].offset_high = (function >> 32) & 0xFFFFFFFF;
    idt[vec].selector = selector;
    idt[vec].attrib = flags;
    idt[vec].ist = 0;
    idt[vec].zero = 0;
}

void irq_eoi(uint8_t irq) {
    if (irq >= 0x20 && irq < 0x30) {
        if (irq >= 0x28)
            Port::outb(0xA0, 0x20);

        Port::outb(0x20, 0x20);
    }
}

extern "C" void irq1_handler() {
    irq_eoi(1);
}