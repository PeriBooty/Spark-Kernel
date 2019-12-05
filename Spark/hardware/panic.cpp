#include <hardware/cpu/cpu.hpp>
#include <lib/lib.hpp>
#include <hardware/panic.hpp>
#include <hardware/terminal.hpp>

void Spark::panic(const char* message) {
    // Draw the text
    Terminal::set_cursor(0, 0);
    Terminal::write_line("!! KERNEL PANIC OCCURRED !!", 0xFFFFFF, 0xe50000);
    char text[255] = "";
    sprintf(text, "Error message: %s", message);
    Terminal::write_line(text, 0xFFFFFF, 0xe50000);
    Cpu::halt_forever();  // Halt the cpu forever
}