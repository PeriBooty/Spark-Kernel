#include <hardware/devices/vbe.hpp>
#include <lib/lib.hpp>
#include <sys/terminal.hpp>

uint16_t Terminal::x = 0;
uint16_t Terminal::y = 0;

/// Handles special characters such as a newline
bool Terminal::handle_special_characters(const char c) {
    bool ret = false;
    switch (c) {
        case '\n': {
            y++;
            ret = true;
            break;
        }
        case '\t': {
            x += 4;
            ret = true;
            break;
        }
        case '\r': {
            x = 0;
            ret = true;
            break;
        }
    }
    return ret;
}

void Terminal::write(const char* str, uint32_t foreground, uint32_t background) {
    uint32_t ln = strlen(str);
    for (uint32_t idx = 0; idx < ln; idx++)
        write(str[idx], foreground, background);
}

void Terminal::write(const char* str, uint32_t foreground) {
    uint32_t ln = strlen(str);
    for (uint32_t idx = 0; idx < ln; idx++)
        write(str[idx], foreground);
}

void Terminal::write(const char c, uint32_t foreground, uint32_t background) {
    if (handle_special_characters(c)) return;
    GraphicsModeInfo mode_info = Display::get_mode_info();
    x++;
    if (x * 8 >= mode_info.width) {
        x = 0;
        y++;
    }
    if (y * 16 >= mode_info.height) {
        x = 0;
        y = mode_info.height / 16 - 1;
    }

    Display::write(c, x * 8, y * 16, foreground, background);
}

void Terminal::write(const char c, uint32_t foreground) {
    if (handle_special_characters(c)) return;
    GraphicsModeInfo mode_info = Display::get_mode_info();
    if (x * 8 >= mode_info.width) {
        x = 0;
        y++;
    }
    if (y * 16 >= mode_info.height) {
        x = 0;
        y = mode_info.height / 16 - 1;
    }

    Display::write(c, x * 8, y * 16, foreground);
    x++;
}

void Terminal::write_line(const char* str, uint32_t foreground, uint32_t background) {
    Terminal::write(str, foreground, background);
    Terminal::write("\r\n", foreground, background);
}

void Terminal::write_line(const char* str, uint32_t foreground) {
    Terminal::write(str, foreground);
    Terminal::write("\r\n", foreground);
}