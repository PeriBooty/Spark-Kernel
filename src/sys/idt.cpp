#include <hardware/port.hpp>
#include <sys/idt.hpp>

IdtEntry idt[256];
IdtPtr idt_ptr;

void idt_init() {
    Port::outb(0x20, 0x11);
    Port::outb(0xA0, 0x11);
    Port::outb(0x21, 0x20);
    Port::outb(0xA1, 0x28);
    Port::outb(0x21, 0x04);
    Port::outb(0xA1, 0x02);
    Port::outb(0x21, 0x01);
    Port::outb(0xA1, 0x01);
    Port::outb(0x21, 0x0);
    Port::outb(0xA1, 0x0);
    mask_irq(0);
    for (uint8_t i = 2; i <= 15; i++)
        mask_irq(i);
    idt_set_gate(34, (uintptr_t)irq1, 0x08, 0x8E);
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

void mask_irq(uint8_t irq) {
    uint16_t port;
    if (irq < 8)
        port = 0x20;
    else {
        port = 0xA0;
        irq -= 8;
    }
    Port::outb(port, Port::inb(port) | (1 << irq));
}

void unmask_irq(uint8_t irq) {
    uint16_t port;
    if (irq < 8)
        port = 0x20;
    else {
        port = 0xA0;
        irq -= 8;
    }
    Port::outb(port, Port::inb(port) & ~(1 << irq));
}