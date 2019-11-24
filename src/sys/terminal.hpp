#pragma once
#include <stdint.h>

namespace Spark::Terminal {
    /**
     * @brief Handles special characters such as a newline
     * 
     * @param c the character
     * @return true 
     * @return false 
     */
    bool handle_special_characters(const char c);

    /**
     * @brief Writes a string on the terminal with a background
     * 
     * @param str the string
     * @param foreground the foreground color
     * @param background the background color
     */
    void write(const char* str, uint32_t foreground, uint32_t background);

    /**
     * @brief Writes a string on the terminal
     * 
     * @param str the string
     * @param foreground the foreground color
     */
    void write(const char* str, uint32_t foreground);

    /**
     * @brief Writes a character on the terminal with a background
     * 
     * @param c the character
     * @param foreground the foreground color
     * @param background the background color
     */
    void write(const char c, uint32_t foreground, uint32_t background);

    /**
     * @brief Writes a character on the terminal
     * 
     * @param c the character
     * @param foreground the foreground color
     */
    void write(const char c, uint32_t foreground);

    /**
     * @brief Writes a string on the terminal with a background then switches to a new line
     * 
     * @param str the string
     * @param foreground the foreground color
     * @param background the background color
     */
    void write_line(const char* str, uint32_t foreground, uint32_t background);

    /**
     * @brief Writes a string on the terminal then switches to a new line
     * 
     * @param str the string
     * @param foreground the foreground color
     */
    void write_line(const char* str, uint32_t foreground);

    /**
     * @brief Sets the cursor position on the terminal
     * 
     * @param x 
     * @param y 
     */
    void set_cursor(uint16_t x, uint16_t y);
};  // namespace Spark::Terminal