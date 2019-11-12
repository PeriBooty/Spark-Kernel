#pragma once
#include <stddef.h>
#include <stdint.h>

/// Graphics mode info struct
struct GraphicsModeInfo {
    uint32_t *framebuffer;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
};

/// Display class/namespace
class Display {
private:
    /// The current graphics mode info
    static GraphicsModeInfo mode_info;

    /// Handles special characters such as a newline
    static bool handle_special_characters(const char c, uint16_t &x, uint16_t &y);

    /// Is the display working?
    static bool is_working;

public:
    /// Initializes the display
    static void init(GraphicsModeInfo &gfx_mode_info);

    /// Clears the display with a specific color
    static void clear(uint32_t color);

    /// Writes a character on a position in the screen
    static void write(const char c, uint16_t x, uint16_t y, uint32_t color);

    /// Writes a string on a postion in the screen
    static void write(const char *str, uint16_t x, uint16_t y, uint32_t color);

    /// Sets a pixel in the display
    static void set_pixel(size_t fb_off, uint32_t color);
    
    /// Sets a pixel in the display
    static void set_pixel(uint16_t x, uint16_t y, uint32_t color);
};