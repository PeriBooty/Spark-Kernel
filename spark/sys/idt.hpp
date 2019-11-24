#pragma once

namespace Spark::Idt {
    /**
     * @brief Interrupt Descriptor Table entry
     * 
     */
    struct [[gnu::packed]] Entry {
        uint16_t offset_low;
        uint16_t selector;
        uint8_t ist;
        uint8_t attrib;
        uint16_t offset_mid;
        uint32_t offset_high;
        uint32_t zero;
    };

    /**
     * @brief Interrupt Descriptor Table pointer
     * 
     */
    struct [[gnu::packed]] Pointer {
        uint16_t limit;
        uint64_t base;
    };

    /**
     * @brief Initializes the Interrupt Descriptor Table
     * 
     */
    void init();

    /**
     * @brief Adds a callback for an interrupt
     * 
     * @param vec 
     * @param function 
     * @param selector 
     * @param flags 
     */
    void set_gate(uint8_t vec, uintptr_t function, uint16_t selector, uint8_t flags);
};  // namespace Spark::Idt