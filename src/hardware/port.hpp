#pragma once
#include <stdint.h>

class Port {
public:
    static void outb(uint16_t port, uint8_t value);
    static uint8_t inb(uint16_t port);
};