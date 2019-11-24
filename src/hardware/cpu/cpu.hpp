#pragma once
#include <stdint.h>

namespace Spark::Cpu {
    /**
     * @brief Checks if MSR is present
     * @param flag
     * @return
     */
    bool check_msr(uint32_t flag);

    /**
     * @brief Halts forever
     */
    void halt_forever();

    /**
     * @brief Locks variable
     * @param var The variable to lock
     */
    void atomic_set(volatile int* var);

    /**
     * @brief Releases variable
     * @param var The variable to release
     */
    void atomic_unset(volatile int* var);
};  // namespace Spark::Cpu