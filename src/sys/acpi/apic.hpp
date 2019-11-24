#pragma once
#include <stdint.h>

namespace Spark::Apic {
    namespace LocalApic {
        constexpr uint64_t icr_low = 0x300;
        constexpr uint64_t icr_high = 0x310;

        enum IcrFlags {
            TM_LEVEL = 0x8000,
            LEVELASSERT = 0x400,
            PENDING = 0x1000,
            DM_INIT = 0x500,
            DM_SIPI = 0x600,
        };

        uint32_t read(uint32_t reg);
        void write(uint32_t reg, uint32_t data);
        void send_eoi();
        void send_ipi(uint32_t target, uint32_t flags);
        void init();
    };  // namespace LocalApic
    // TODO: Implement other APIC things
};  // namespace Spark::Apic