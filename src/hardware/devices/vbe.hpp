#pragma once
#include <stdint.h>
#include <stddef.h>

struct VideoModeInfo {
    uint32_t *framebuffer;
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
    static void clear(uint32_t color);
    static void write(const char c, uint16_t x, uint16_t y, uint32_t color);
    static void write(const char* str, uint16_t x, uint16_t y, uint32_t color);
    static void set_pixel(size_t fb_off, uint32_t color);
};