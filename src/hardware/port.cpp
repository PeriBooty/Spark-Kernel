#include <hardware/port.hpp>

void Port::outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1"
                   :
                   : "a"(val), "Nd"(port));
}

uint8_t Port::inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port));
    return ret;
}