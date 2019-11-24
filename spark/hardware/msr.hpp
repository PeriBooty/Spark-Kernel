#pragma once
#include <stdint.h>

namespace Spark::Msr {
    constexpr uint32_t ia32_pat = 0x277;
    constexpr uint32_t ia32_tme_capability = 0x981;
    constexpr uint32_t ia32_tme_activate = 0x982;
    constexpr uint32_t ia32_tme_exclude_mask = 0x983;
    constexpr uint32_t ia32_tme_exclude_base = 0x984;
    constexpr uint32_t ia32_efer = 0xC0000080;
    constexpr uint32_t apic_base = 0x0000001b;
    constexpr uint32_t fs_base = 0xC0000100;
    constexpr uint32_t gs_base = 0xC0000101;
    constexpr uint32_t kernelgs_base = 0xC0000102;

    /**
     * @brief Gets MSR
     * @param msr The MSR
     * @return The value of the MSR
     */
    uint64_t read(uint32_t msr);

    /**
     * @brief Sets MSR
     * @param msr The MSR
     * @param value The value to set the MSR to
     */
    void write(uint32_t msr, uint64_t value);
}  // namespace Spark::Cpu::Msr