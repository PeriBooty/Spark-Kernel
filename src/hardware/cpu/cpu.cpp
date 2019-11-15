#include <cpuid.h>
#include <hardware/cpu/cpu.hpp>

/// Checks if CPU PAT MSR is supported
bool cpu_check_pat() {
    uint32_t a, b, c, d;
    __cpuid(1, a, b, c, d);
    if (d & (1 << 16))
        return true;
    else
        return false;
}

/// Halts CPU forever
void cpu_halt_forever() {
    asm volatile(
        "cli\n"
        "1:\n"
        "hlt\n"
        "jmp 1b"
        :
        :
        : "memory");
}

// Sets MSR
void cpu_set_msr(uint32_t msr, uint64_t value) {
    uint32_t low = static_cast<uint32_t>(value);
    uint32_t high = static_cast<uint32_t>(value >> 32);
    asm volatile("wrmsr"
                 :
                 : "a"(low), "d"(high), "c"(msr));
}