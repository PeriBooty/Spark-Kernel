#include <hardware/devices/font.hpp>
#include <hardware/devices/vbe.hpp>
#include <hardware/mm/mm.hpp>
#include <lib/lib.hpp>

VideoModeInfo Display::mode_info{};

void Display::init(VideoModeInfo &gfx_mode_info) {
    Display::mode_info = gfx_mode_info;
}

void Display::clear(uint32_t color) {
    memset(mode_info.framebuffer, color, mode_info.width * mode_info.pitch);
}

bool Display::handle_special_characters(const char c, uint16_t &x, uint16_t &y) {
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

void Display::write(const char c, uint16_t x, uint16_t y, uint32_t color) {
    if (c == '\0') return;
    if (handle_special_characters(c, x, y)) return;
    size_t font_off = (size_t)c * uni_vga_font.height * uni_vga_font.width / 8;
    size_t fb_off = ((x * uni_vga_font.width) * 4 + (y * uni_vga_font.height) * mode_info.pitch) / 4;
    for (uint32_t ny = 0; ny < uni_vga_font.height; ny++) {
        size_t tmp_fb_off = fb_off;
        for (uint32_t nx = 0; nx < uni_vga_font.width; nx++) {
            if (uni_vga_font.bitmap[font_off + ny] & (1 << (8 - nx)))
                set_pixel(tmp_fb_off, color);
            tmp_fb_off++;
        }
        fb_off += mode_info.pitch / 4;
    }
}

void Display::write(const char *str, uint16_t x, uint16_t y, uint32_t color) {
    uint32_t ln = strlen(str);
    for (uint32_t idx = 0; idx < ln; idx++) {
        if (handle_special_characters(str[idx], x, y)) continue;
        write(str[idx], x, y, color);
    }
}

void Display::set_pixel(size_t fb_off, uint32_t color) {
    mode_info.framebuffer[fb_off] = color;
}