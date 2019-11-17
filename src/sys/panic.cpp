#include <hardware/cpu/cpu.hpp>
#include <sys/terminal.hpp>
#include <sys/panic.hpp>
#include <lib/lib.hpp>

/// Kernel panic function
void panic(const char* message) {
    // Draw the text
    Terminal::set_cursor(0, 0);
    Terminal::write_line("!! KERNEL PANIC OCCURRED !!", 0xFFFFFF, 0x000000);
    printf("Error message: %s", 0xFFFFFF, 0x000000, message);
    CentralProcessingUnit::halt_forever();  // Halt the cpu forever
}