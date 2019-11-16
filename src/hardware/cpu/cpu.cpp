#include <cpuid.h>
#include <hardware/cpu/cpu.hpp>

/// Checks if CPU PAT MSR is supported
bool CPU::check_pat() {
    uint32_t a, b, c, d;
    __cpuid(1, a, b, c, d);
    return (d & (1 << 16));
}

/// Halts CPU forever
void CPU::halt_forever() {
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
void CPU::set_msr(uint32_t msr, uint64_t value) {
    uint32_t low = static_cast<uint32_t>(value);
    uint32_t high = static_cast<uint32_t>(value >> 32);
    asm volatile("wrmsr"
                 :
                 : "a"(low), "d"(high), "c"(msr));
}

/// Locks and waits for release of int var pointer
void CPU::atomic_loop_test_and_set(volatile int* var) {
    asm volatile(
        "1:\n\t"
        "lock bts $0, %0\n\t"
        "jnc 2f\n\t"
        "pause\n\t"
        "jmp 1b\n\t"
        "2:\n\t"
        : "+m"(*var)
        :
        : "memory", "cc");
}

/// Releases int var pointer
void CPU::atomic_unset(volatile int* var) {
    asm volatile("lock btr $0, %0\n\t"
                 : "+m"(*var)
                 :
                 : "memory", "cc");
}