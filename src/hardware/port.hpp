#pragma once
#include <stdint.h>

class Port {
public:
    static void outb(uint16_t port, uint8_t value);
    static uint8_t inb(uint16_t port);
    static void outw(uint16_t port, uint16_t value);
    static uint16_t inw(uint16_t port);
    static void outd(uint16_t port, uint32_t value);
    static uint32_t ind(uint16_t port);
    static void wait();
};