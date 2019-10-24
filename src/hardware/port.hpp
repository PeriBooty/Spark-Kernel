#pragma once
#include <stdint.h>

class Port {
public:
    static void outb(uint16_t, uint8_t);
    static uint8_t inb(uint16_t);
};