#include <cpuid.h>
#include <hardware/cpu/cpu.hpp>

bool Spark::Cpu::check_msr(uint32_t flag) {
    uint32_t a, b, c, d;
    __cpuid(1, a, b, c, d);

    return d & flag;
}

void Spark::Cpu::halt_forever() {
    asm volatile(
        "cli\n"
        "1:\n"
        "hlt\n"
        "jmp 1b"
        :
        :
        : "memory");
}

void Spark::Cpu::atomic_set(volatile int* var) {
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

void Spark::Cpu::atomic_unset(volatile int* var) {
    asm volatile("lock btr $0, %0\n\t"
                 : "+m"(*var)
                 :
                 : "memory", "cc");
}