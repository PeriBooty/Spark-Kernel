#pragma once
#include <stdint.h>

namespace Spark::Cpu::Smp {
    struct CpuEntry {
        uint32_t lapic_id;
        bool bsp;
    };

    /**
     * @brief Get how long the trampoline is
     * 
     * @return uint64_t 
     */
    uint64_t get_len();

    /**
     * @brief Waits & timeouts for the AP to boot
     * 
     * @return true 
     * @return false 
     */
    bool wait_for_boot();

    /**
     * @brief Maps stuff to prepare to initialize CPUs
     * 
     */
    void init();

    /**
     * @brief Boots a cpu up
     * 
     * @param cpu 
     */
    void boot_cpu(CpuEntry cpu);

    /**
     * @brief Sets the trampoline_booted variable to true
     * 
     */
    void set_booted();
}  // namespace Spark::Cpu::Smp