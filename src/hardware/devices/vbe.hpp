#pragma once
#include <stdint.h>
#include <stddef.h>
#define background = 0x00000000;
#define foreground = 0xA8A8A8A8;

struct VideoModeInfo {
    uint64_t *framebuffer;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
};

class Display {
private:
    static VideoModeInfo mode_info;
    static bool handle_special_characters(const char c, uint16_t &x, uint16_t &y);

public:
    static void init(VideoModeInfo &gfx_mode_info);
    static void clear(uint16_t color);
    static void write(const char c, uint16_t x, uint16_t y, uint16_t color);
    static void write(const char* str, uint16_t x, uint16_t y, uint16_t color);
    static void set_pixel(size_t fb_off, uint16_t color);
};