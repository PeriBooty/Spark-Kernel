#include <hardware/cpu/lock.hpp>

/// Locks and waits for release of int var pointer
void cpu_atomic_loop_test_and_set(volatile int *var) {
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
void cpu_atomic_unset(volatile int *var) {
    asm volatile("lock btr $0, %0\n\t"
                 : "+m"(*var)
                 :
                 : "memory", "cc");
}