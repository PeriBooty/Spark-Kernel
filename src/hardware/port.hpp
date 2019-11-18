#pragma once
#include <stdint.h>

/// Port class/namespace
namespace Spark::Port {
    /// 8bit output to port
    void outb(uint16_t port, uint8_t value);

    /// 8bit port input
    uint8_t inb(uint16_t port);

    /// 16bit << WORD >> output to port
    void outw(uint16_t port, uint16_t value);

    /// 16bit << WORD >> port input
    uint16_t inw(uint16_t port);

    /// 32bit << DWORD >> output to port
    void outd(uint16_t port, uint32_t value);

    /// 32bit << DWORD >> port input
    uint32_t ind(uint16_t port);

    /// Waits for I/O to finish
    void wait();
};  // namespace Spark::Port