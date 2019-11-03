#include <hardware/cpu/cpu.hpp>

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