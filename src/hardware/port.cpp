#include <hardware/port.hpp>

void Port::outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1\n\t"
                 :
                 : "a"(value), "Nd"(port));
}

uint8_t Port::inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0\n\t"
                 : "=a"(ret)
                 : "Nd"(port));
    return ret;
}