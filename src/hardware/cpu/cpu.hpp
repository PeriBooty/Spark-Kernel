#pragma once
#include <stdint.h>

bool cpu_check_pat();
void cpu_halt_forever();
void cpu_set_msr(uint32_t msr, uint64_t value);