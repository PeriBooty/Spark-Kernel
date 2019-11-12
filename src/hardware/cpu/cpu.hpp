#pragma once
#include <stdint.h>

/// Checks if CPU PAT MSR is supported
bool cpu_check_pat();

/// Halts CPU forever
void cpu_halt_forever();

/// Sets MSR
void cpu_set_msr(uint32_t msr, uint64_t value);