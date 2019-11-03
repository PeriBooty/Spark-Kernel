#include <hardware/devices/vbe.hpp>
#include <hardware/cpu/cpu.hpp>
#include <sys/panic.hpp>

void panic(const char *message) {
    Display::clear(0x3C59FF);
    Display::write(":( Sorry, an error has occured.", 0, 16, 0xFFFFFF);
    Display::write("The OS has stopped to prevent damage to your PC.", 0, 32, 0xFFFFFF);
    Display::write("Error code:", 0, 64, 0xFFFFFF);
    Display::write(message, 12 * 8, 64, 0xFFFFFF);
    cpu_halt_forever();
}