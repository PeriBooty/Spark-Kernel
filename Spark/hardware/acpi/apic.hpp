#pragma once
#include <stdint.h>

namespace Spark::Apic {
    namespace LocalApic {
        constexpr uint64_t icr_low = 0x300;
        constexpr uint64_t icr_high = 0x310;

        enum IcrFlags {
            TM_LEVEL = 0x8000,
            LEVELASSERT = 0x4000,
            PENDING = 0x1000,
            DM_INIT = 0x500,
            DM_SIPI = 0x600,
        };

        /**
         * @brief Reads from the apic
         * 
         * @param reg 
         * @return uint32_t 
         */
        uint32_t read(uint32_t reg);

        /**
         * @brief Writes to the apic
         * 
         * @param reg 
         * @param data 
         */
        void write(uint32_t reg, uint32_t data);

        /**
         * @brief Sends << END OF INTERRUPT >> to the lapic
         * 
         */
        void send_eoi();

        /**
         * @brief Sends IPI to apic
         * 
         * @param target 
         * @param flags 
         */
        void send_ipi(uint32_t target, uint32_t flags);

        /**
         * @brief Initializes lapic
         * 
         */
        void init();
    };  // namespace LocalApic
    // TODO: Implement other APIC things
};  // namespace Spark::Apic