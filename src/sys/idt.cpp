#include <sys/terminal.hpp>
#include <hardware/port.hpp>
#include <lib/lib.hpp>
#include <sys/idt.hpp>

IdtEntry idt[256];  // Interrupt Descriptor Table
IdtPtr idt_ptr;     // Interrupt Descriptor Table pointer
int ticks;          // PIT total ticks

/// Initializes the Interrupt Descriptor Table
void idt_init() {
    // Remap PIC
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
    Port::outb(0x21, 0xff);
    Port::wait();
    Port::outb(0xA1, 0xff);
    Port::wait();

    // Mask all IRQs
    Port::outb(0xa1, 0);
    Port::outb(0x21, 0);

    // Unmask some IRQs
    unmask_irq(0);
    unmask_irq(1);

    idt_set_gate(32, reinterpret_cast<uintptr_t>(irq0), 0x08, 0x8E);  // Set callback for IRQ 0
    idt_set_gate(33, reinterpret_cast<uintptr_t>(irq1), 0x08, 0x8E);  // Set callback for IRQ 1
    pit_set_frequency(10000);                       // Set PIT frequency to 1MHz
    Terminal::write_line("[PIT] Set frequency to 1MHz", 0xFFFFFF);
    idt_ptr.limit = 256 * sizeof(IdtEntry) - 1;     // Set Interrupt Descriptor Table pointer size
    idt_ptr.base = reinterpret_cast<uint64_t>(&idt);                  // Set Interrupt Descriptor Table pointer address
    asm volatile("lidt %0" ::"m"(idt_ptr));         // Load Interrupt Descriptor Table pointer
    asm volatile("sti");                            // Enable interrupts
    Terminal::write_line("[IDT] Initialized IDT successfully", 0xFFFFFF);
}

/// Adds a callback for an interrupt
void idt_set_gate(uint8_t vec, uintptr_t function, uint16_t selector, uint8_t flags) {
    idt[vec].offset_low = function & 0xFFFF;               // Set low offset
    idt[vec].offset_mid = (function >> 16) & 0xFFFF;       // Set middle offset
    idt[vec].offset_high = (function >> 32) & 0xFFFFFFFF;  // Set high offset
    idt[vec].selector = selector;                          // Set selector
    idt[vec].attrib = flags;                               // Set flags
    idt[vec].ist = 0;                                      // IST number
    idt[vec].zero = 0;                                     // Always zero
}

/// Sends to the PIC master and/or the slave an << END OF INTERRUPT >>
void irq_eoi(uint8_t irq) {
    if (irq >= 8)
        Port::outb(0xA0, 0x20);  // If IRQ number is more than 8, send << END OF INTERRUPT >> to slave PIC

    Port::outb(0x20, 0x20);  // Send << END OF INTERRUPT >> to master PIC
}

// Masks/Disables an IRQ
void mask_irq(uint8_t irq) {
    uint16_t port = 0x21;  // Default port is master PIC

    // If IRQ number is more than 8, send command to slave PIC
    if (irq >= 8) {
        port = 0xA1;
        irq -= 8;
    }

    Port::outb(port, Port::inb(port) | (1 << irq));  // Tell master/slave PIC to mask IRQ
}

// Unmasks/Enables an IRQ
void unmask_irq(uint8_t irq) {
    uint16_t port = 0x21;  // Default port is master PIC

    // If IRQ number is more than 8, send command to slave PIC
    if (irq >= 8) {
        port = 0xA1;
        irq -= 8;
    }

    Port::outb(port, Port::inb(port) & ~(1 << irq));  // Tell master/slave PIC to unmask IRQ
}

/// Changes the PIT's frequency
void pit_set_frequency(uint32_t frequency) {
    uint32_t divisor = 1193182 / frequency;

    Port::outb(0x43, 0x36);
    Port::wait();
    Port::outb(0x40, static_cast<uint8_t>(divisor & 0xFF));
    Port::wait();
    Port::outb(0x40, static_cast<uint8_t>((divisor >> 8)));
}

// IRQ1/Keyboard interrupt handler, called by boot.asm
extern "C" void irq1_handler() {
    //uint8_t scancode = Port::inb(0x60); // Poll scancode
    irq_eoi(1);
}

// IRQ0/PIT interrupt handler, called by boot.asm
extern "C" void pit_handler() {
    // Do nothing
    irq_eoi(0);
}