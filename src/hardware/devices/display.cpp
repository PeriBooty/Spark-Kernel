#include <hardware/devices/display.hpp>
#include <hardware/devices/font.hpp>
#include <hardware/port.hpp>
#include <lib/lib.hpp>

uint16_t Display::width = 1280;
uint16_t Display::height = 720;
uint8_t Display::bpp = 0;
uint16_t Display::x = 0;
uint16_t Display::y = 0;
VGAColor Display::background = VGAColor::BLACK;
VGAColor Display::foreground = VGAColor::LIGHT_GREEN;
bool Display::cursor_enabled = false;
uint64_t* Display::display_buffer = nullptr;
extern BitmapFont uni_vga_font;

void Display::init(uint64_t fb, uint16_t width, uint16_t height, uint8_t bpp) {
    display_buffer = (uint64_t*)fb;
    Display::width = width;
    Display::height = height;
    Display::bpp = bpp;
    //clear();
    toggle_cursor(true);
}

void Display::clear() {
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            display_buffer[y * width + x] = 0;
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
            display_buffer[height * width + i] = 0;
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
    if (c == '\0') return;
    x++;
    set_cursor_pos(x, y);
    if (handle_special_characters(c)) return;
    for (uint16_t ny = 0; ny < 8; ny++) {
        for (uint16_t nx = 8; nx > 0; nx--) {
            if ((uni_vga_font.bitmap[ny] & (1 << nx))) {
                set_pixel(y + ny, x + nx, foreground);
            }
        }
    }
    handle_write_pos();
}

void Display::set_pixel(uint16_t x, uint16_t y, uint16_t color) {
    uint32_t where = x * 4 + y * (bpp * 100);
    display_buffer[where] = color & 255;
    display_buffer[where + 1] = (color >> 8) & 255;
    display_buffer[where + 2] = (color >> 16) & 255;
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