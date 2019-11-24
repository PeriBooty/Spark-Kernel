#pragma once
#include <stdint.h>

namespace Spark::Port {
    /**
     * @brief 8bit output to port
     * 
     * @param port 
     * @param value 
     */
    void outb(uint16_t port, uint8_t value);

    /**
     * @brief 8bit port input
     * 
     * @param port 
     * @return uint8_t 
     */
    uint8_t inb(uint16_t port);

    /**
     * @brief 16bit << WORD >> output to port
     * 
     * @param port 
     * @param value 
     */
    void outw(uint16_t port, uint16_t value);

    /**
     * @brief 16bit << WORD >> port input
     * 
     * @param port 
     * @return uint16_t 
     */
    uint16_t inw(uint16_t port);

    /**
     * @brief 32bit << DWORD >> output to port
     * 
     * @param port 
     * @param value 
     */
    void outd(uint16_t port, uint32_t value);

    /**
     * @brief 32bit << DWORD >> port input
     * 
     * @param port 
     * @return uint32_t 
     */
    uint32_t ind(uint16_t port);

    /**
     * @brief Waits for I/O to finish
     * 
     */
    void wait();
};  // namespace Spark::Port