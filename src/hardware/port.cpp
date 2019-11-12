#include <hardware/port.hpp>

/// 8bit output to port
void Port::outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1"
                 :
                 : "a"(value), "Nd"(port));
}

/// 8bit port input
uint8_t Port::inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));
    return ret;
}

/// 16bit << WORD >> output to port
void Port::outw(uint16_t port, uint16_t value) {
    asm volatile("outw %0, %1"
                 :
                 : "a"(value), "Nd"(port));
}

/// 16bit << WORD >> port input
uint16_t Port::inw(uint16_t port) {
    uint16_t ret;
    asm volatile("inw %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));
    return ret;
}

/// 32bit << DWORD >> output to port
void Port::outd(uint16_t port, uint32_t value) {
    asm volatile("outl %0, %1"
                 :
                 : "a"(value), "Nd"(port));
}

/// 32bit << DWORD >> port input
uint32_t Port::ind(uint16_t port) {
    uint32_t ret;
    asm volatile("inl %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));
    return ret;
}

/// Waits for I/O to finish
void Port::wait() {
    outb(0x80, 0);
}