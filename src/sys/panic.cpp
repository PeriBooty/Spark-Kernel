#include <hardware/cpu/cpu.hpp>
#include <lib/lib.hpp>
#include <sys/panic.hpp>
#include <sys/terminal.hpp>

void Spark::panic(const char* message) {
    // Draw the text
    Terminal::set_cursor(0, 0);
    Terminal::write_line("!! KERNEL PANIC OCCURRED !!", 0xFFFFFF, 0x000000);
    char text[255] = "";
    sprintf(text, "Error message: %s", message);
    Terminal::write_line(text, 0xFFFFFF, 0x000000);
    //Cpu::halt_forever();  // Halt the cpu forever
}