#include <hardware/devices/display.hpp>
#include <hardware/port.hpp>
#include <lib/lib.hpp>

uint16_t Display::width = 1280;
uint16_t Display::height = 720;
uint16_t Display::x = 0;
uint16_t Display::y = 0;
VGAColor Display::background = VGAColor::BLACK;
VGAColor Display::foreground = VGAColor::LIGHT_GREEN;
bool Display::cursor_enabled = false;
uint16_t* Display::display_buffer = nullptr;

void Display::init(uint64_t fb, uint16_t width, uint16_t height) {
    display_buffer = (uint16_t*)fb;
    Display::width = width;
    Display::height = height;
    clear();
    toggle_cursor(true);
}

uint16_t Display::prepare_char(const char c) {
    return c | (((background << 4) | (foreground & 0x0F)) << 8);
}

void Display::clear() {
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            display_buffer[y * width + x] = prepare_char(' ');
        }
    }
    x = 0;
    y = 0;
}

void Display::handle_write_pos() {
    if (x >= width) {
        y++;
        x = 0;
    }
    if (y >= height) {
        copy(display_buffer + width, display_buffer + ((height - 1) * width), display_buffer);
        y = height - 1;
        x = 0;
        for (uint8_t i = 0; i < width; i++)
            display_buffer[height * width + i] = prepare_char(' ');
    }
    set_cursor_pos(x, y);
}

bool Display::handle_special_characters(const char c) {
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
    if (ret) handle_write_pos();
    return ret;
}

void Display::write(const char c) {
    if (c == '\0')
        return;
    set_cursor_pos(x, y);
    if (handle_special_characters(c)) return;
    display_buffer[y * width + x++] = prepare_char(c);
    handle_write_pos();
}

void Display::write(const char* str) {
    uint32_t ln = strlen(str);
    for (uint32_t idx = 0; idx < ln; idx++) {
        write(str[idx]);
    }
}

void Display::write_line(const char* str) {
    write(str);
    write("\r\n");
}

void Display::write_line(const char c) {
    write(c);
    write("\r\n");
}

bool Display::is_cursor_enabled() {
    return cursor_enabled;
}

void Display::toggle_cursor(bool enabled) {
    cursor_enabled = enabled;
    // to be implemented in another galaxy
}

void Display::set_cursor_pos(int newx, int newy) {
    if (newx > width || newy > height)
        return;
    x = newx;
    y = newy;
    // to be implemented in another galaxy
}

void Display::backspace() {
    if (y > 0 && x <= 0) {
        x = width - 1;
        y--;
    } else if (x > 0)
        x--;
    // to be implemented in another galaxy
    set_cursor_pos(x, y);
}