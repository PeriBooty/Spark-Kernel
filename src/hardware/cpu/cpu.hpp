#pragma once
#include <stdint.h>

namespace Spark::Cpu {
    /// Checks if CPU PAT MSR is supported
    bool check_pat();

    /// Halts CPU forever
    void halt_forever();

    /// Sets MSR
    void set_msr(uint32_t msr, uint64_t value);

    /// Locks and waits for release of int var pointer
    void atomic_set(volatile int* var);

    /// Releases int var pointer
    void atomic_unset(volatile int* var);
};  // namespace Spark::Cpu