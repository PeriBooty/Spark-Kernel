#include <hardware/cpu/cpu.hpp>
#include <hardware/devices/vbe.hpp>
#include <sys/panic.hpp>

/// Kernel panic function
void kernel_panic(const char *message) {
    Display::clear(0x0000FF);  // Make the display background blue
    // Draw the text
    Display::write(":( Sorry, an error has occured.", 0, 16, 0xFFFFFF);
    Display::write("The OS has stopped to prevent damage to your PC.", 0, 32, 0xFFFFFF);
    Display::write("Error message:", 0, 64, 0xFFFFFF);
    Display::write(message, 12 * 8, 64, 0xFFFFFF);
    cpu_halt_forever();  // Halt the cpu forever
}