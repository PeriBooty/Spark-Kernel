#pragma once
#include <stdint.h>

/// Port class/namespace
class Port {
public:
    /// 8bit output to port
    static void outb(uint16_t port, uint8_t value);

    /// 8bit port input
    static uint8_t inb(uint16_t port);

    /// 16bit << WORD >> output to port
    static void outw(uint16_t port, uint16_t value);

    /// 16bit << WORD >> port input
    static uint16_t inw(uint16_t port);

    /// 32bit << DWORD >> output to port
    static void outd(uint16_t port, uint32_t value);

    /// 32bit << DWORD >> port input
    static uint32_t ind(uint16_t port);

    /// Waits for I/O to finish
    static void wait();
};