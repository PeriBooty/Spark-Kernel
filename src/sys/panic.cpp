#include <hardware/cpu/cpu.hpp>
#include <hardware/devices/vbe.hpp>
#include <sys/panic.hpp>

/// Kernel panic function
void panic(const char* message) {
    // Draw the text
    Display::write(":( Sorry, an error has occured.", 0, 16, 0xFFFFFF);
    Display::write("The OS has stopped to prevent damage to your PC.", 0, 32, 0xFFFFFF);
    Display::write("Error message:", 0, 64, 0xFFFFFF);
    Display::write(message, 15 * 8, 64, 0xFFFFFF);
    CPU::halt_forever();  // Halt the cpu forever
}