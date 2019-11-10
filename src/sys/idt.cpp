#include <hardware/devices/vbe.hpp>
#include <hardware/port.hpp>
#include <lib/lib.hpp>
#include <sys/idt.hpp>

IdtEntry idt[256];
IdtPtr idt_ptr;
int ticks;

void idt_init() {
    Port::outb(0x20, 0x11);
    Port::wait();
    Port::outb(0xA0, 0x11);
    Port::wait();
    Port::outb(0x21, 32);
    Port::wait();
    Port::outb(0xA1, 40);
    Port::wait();
    Port::outb(0x21, 4);
    Port::wait();
    Port::outb(0xA1, 2);
    Port::wait();
    Port::outb(0x21, 0x01);
    Port::wait();
    Port::outb(0xA1, 0x01);
    Port::wait();
    Port::outb(0xa1, 0xff);
    Port::outb(0x21, 0xff);
    unmask_irq(0);
    unmask_irq(1);
    idt_set_gate(32, (uintptr_t)irq0, 0x08, 0x8E);
    idt_set_gate(33, (uintptr_t)irq1, 0x08, 0x8E);
    init_pit(10000);
    idt_ptr.limit = 256 * sizeof(IdtEntry) - 1;
    idt_ptr.base = (uint64_t)&idt;
    asm volatile("lidt %0" ::"m"(idt_ptr));
    asm volatile("sti");
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
    if (irq >= 8)
        Port::outb(0xA0, 0x20);

    Port::outb(0x20, 0x20);
}

extern "C" void irq1_handler() {
    irq_eoi(1);
}

extern "C" void pit_handler() {
    ticks++;
    Display::clear(0x000000);
    Display::write("[PIT] Ticked", 0, 0, 0xFFFFFF);
    char* ticks_str = (char*)"";
    itoa(ticks, ticks_str, 10);
    Display::write(ticks_str, 13 * 8, 0, 0xFFFFFF);
    Display::write(" times.", strlen(ticks_str) * 8, 0, 0xFFFFFF);
    irq_eoi(0);
}

void mask_irq(uint8_t irq) {
    uint16_t port = 0xA1;
    if (irq >= 8) {
        port = 0x21;
        irq -= 8;
    }
    Port::outb(port, Port::inb(port) | (1 << irq));
}

void unmask_irq(uint8_t irq) {
    uint16_t port = 0xA1;
    if (irq >= 8) {
        port = 0x21;
        irq -= 8;
    }
    Port::outb(port, Port::inb(port) & ~(1 << irq));
}

void init_pit(uint32_t frequency) {
    uint32_t divisor = 1193182 / frequency;
    Port::outb(0x43, 0x36);
    Port::wait();
    Port::outb(0x40, (uint8_t)(divisor & 0xFF));
    Port::wait();
    Port::outb(0x40, (uint8_t)((divisor >> 8)));
}