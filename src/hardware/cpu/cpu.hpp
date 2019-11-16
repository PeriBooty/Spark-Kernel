#pragma once
#include <stdint.h>

class CPU {
public:
    /// Checks if CPU PAT MSR is supported
    static bool check_pat();

    /// Halts CPU forever
    static void halt_forever();

    /// Sets MSR
    static void set_msr(uint32_t msr, uint64_t value);

    /// Locks and waits for release of int var pointer
    static void atomic_loop_test_and_set(volatile int* var);

    /// Releases int var pointer
    static void atomic_unset(volatile int* var);
};