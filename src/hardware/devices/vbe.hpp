#pragma once
#include <stddef.h>
#include <stdint.h>

/// Display class/namespace
namespace Spark::Graphics {
    /**
     * @brief Graphics mode info
     * 
     */
    struct ModeInfo {
        uint32_t* backbuffer;
        uint32_t* framebuffer;
        uint32_t pitch;
        uint32_t width;
        uint32_t height;
        uint8_t bpp;
    };

    /**
     * @brief Initializes the display
     * 
     * @param gfx_mode_info the info for the current graphics mode
     */
    void init(ModeInfo gfx_mode_info);

    /**
     * @brief Clears the display with a specific color
     * 
     * @param color the color to clear it with
     */
    void clear(uint32_t color);

    /**
     * @brief Writes a character on a position in the screen
     * 
     * @param c the character
     * @param x the x position
     * @param y the y position
     * @param foreground the foreground color
     */
    void write_text(const char c, uint16_t x, uint16_t y, uint32_t foreground);

    /**
     * @brief Writes a string on a postion in the screen
     * 
     * @param str the string
     * @param x the x position
     * @param y the y position
     * @param foreground the foreground color
     */
    void write_text(const char* str, uint16_t x, uint16_t y, uint32_t foreground);

    /**
     * @brief Writes a character on a position in the screen with background color
     * 
     * @param c the character
     * @param x the x position
     * @param y the y position
     * @param foreground the foreground color
     * @param background the background color
     */
    void write_text(const char c, uint16_t x, uint16_t y, uint32_t foreground, uint32_t background);

    /**
     * @brief Writes a string on a postion in the screen with background color
     * 
     * @param str the string
     * @param x the x position
     * @param y the y position
     * @param foreground the foreground color
     * @param background the background color
     */
    void write_text(const char* str, uint16_t x, uint16_t y, uint32_t foreground, uint32_t background);

    /**
     * @brief Sets a pixel on the screen
     * 
     * @param x the x position
     * @param y the y position
     * @param color the color
     */
    inline void set_pixel(uint16_t x, uint16_t y, uint32_t color);
    
    /**
     * @brief Get the mode info object
     * 
     * @return ModeInfo 
     */
    ModeInfo get_mode_info();

    /**
     * @brief Refreshes screen
     * 
     */
    void update();
};  // namespace Spark::Graphics