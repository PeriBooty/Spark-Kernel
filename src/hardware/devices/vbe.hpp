#pragma once
#include <stddef.h>
#include <stdint.h>

/// Display class/namespace
namespace Spark::Graphics {
    /// Graphics mode info struct
    struct ModeInfo {
        uint32_t* backbuffer;
        uint32_t* framebuffer;
        uint32_t pitch;
        uint32_t width;
        uint32_t height;
        uint8_t bpp;
    };

    /// Initializes the display
    void init(ModeInfo gfx_mode_info);

    /// Clears the display with a specific color
    void clear(uint32_t color);

    /// Writes a character on a position in the screen
    void write_text(const char c, uint16_t x, uint16_t y, uint32_t color);

    /// Writes a string on a postion in the screen
    void write_text(const char* str, uint16_t x, uint16_t y, uint32_t color);

    /// Writes a character on a position in the screen with background color
    void write_text(const char c, uint16_t x, uint16_t y, uint32_t color, uint32_t background);

    /// Writes a string on a postion in the screen with background color
    void write_text(const char* str, uint16_t x, uint16_t y, uint32_t color, uint32_t background);

    /// Sets a pixel in the display
    inline void set_pixel(uint16_t x, uint16_t y, uint32_t color);
    ModeInfo get_mode_info();
    void update();
};  // namespace Spark::Graphics