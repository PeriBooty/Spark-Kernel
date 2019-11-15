#include <hardware/devices/font.hpp>
#include <hardware/devices/vbe.hpp>
#include <hardware/mm/mm.hpp>
#include <lib/lib.hpp>
#include <sys/terminal.hpp>

GraphicsModeInfo Display::mode_info{};  // Mode info is empty
bool Display::is_working = false;       // Display is initially not working of course

/// Initializes the display
void Display::init(GraphicsModeInfo &gfx_mode_info) {
    is_working = false;
    Display::mode_info = gfx_mode_info;
    is_working = true;
    Display::clear(0x000000);
    Terminal::write_line("[Display] Initialized display successfully", 0xFFFFFF);
}

/// Clears the display with a specific color
void Display::clear(uint32_t color) {
    if (!is_working) return;
    for (uint32_t y = 0; y < mode_info.height; y++)
        for (uint32_t x = 0; x < mode_info.width; x++)
                set_pixel(x, y, color);
}

/// Writes a character on a position in the screen
void Display::write(const char c, uint16_t x, uint16_t y, uint32_t color) {
    if (!is_working || c == '\0') return;
    size_t font_off = static_cast<size_t>(c) * display_font.height * display_font.width / 8;
    for (uint32_t ny = 0; ny < display_font.height; ny++)
        for (uint32_t nx = 0; nx < display_font.width; nx++)
            if (display_font.bitmap[font_off + ny] & (1 << (8 - nx)))
                set_pixel(x + nx, y + ny, color);
}

/// Writes a string on a position in the screen
void Display::write(const char *str, uint16_t x, uint16_t y, uint32_t color) {
    if (!is_working) return;
    uint32_t ln = strlen(str);
    for (uint32_t idx = 0; idx < ln; idx++) {
        x += display_font.width;
        write(str[idx], x, y, color);
    }
}

/// Writes a character on a position in the screen
void Display::write(const char c, uint16_t x, uint16_t y, uint32_t color, uint32_t background) {
    if (!is_working || c == '\0') return;
    size_t font_off = static_cast<size_t>(c) * display_font.height * display_font.width / 8;
    for (uint32_t ny = 0; ny < display_font.height; ny++)
        for (uint32_t nx = 0; nx < display_font.width; nx++)
            set_pixel(x + nx, y + ny, display_font.bitmap[font_off + ny] & (1 << (8 - nx)) ? color : background);
}

/// Writes a string on a position in the screen
void Display::write(const char *str, uint16_t x, uint16_t y, uint32_t color, uint32_t background) {
    if (!is_working) return;
    uint32_t ln = strlen(str);
    for (uint32_t idx = 0; idx < ln; idx++) {
        x += display_font.width;
        write(str[idx], x, y, color, background);
    }
}

/// Sets a pixel in the display
void inline Display::set_pixel(uint16_t x, uint16_t y, uint32_t color) {
    if (!is_working) return;
    mode_info.framebuffer[(x * 4 + y * mode_info.pitch) / 4] = color;
}

GraphicsModeInfo Display::get_mode_info() {
    return mode_info;
}