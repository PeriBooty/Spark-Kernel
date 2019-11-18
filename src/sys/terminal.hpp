#pragma once
#include <stdint.h>

namespace Spark::Terminal {
    /// Handles special characters such as a newline
    bool handle_special_characters(const char c);

    /// Writes a string on the terminal with a background
    void write(const char* str, uint32_t foreground, uint32_t background);

    /// Writes a string on the terminal
    void write(const char* str, uint32_t foreground);

    /// Writes a character on the terminal with a background
    void write(const char c, uint32_t foreground, uint32_t background);

    /// Writes a character on the terminal
    void write(const char c, uint32_t foreground);

    /// Writes a string on the terminal with a background then switches to a new line
    void write_line(const char* str, uint32_t foreground, uint32_t background);

    /// Writes a string on the terminal then switches to a new line
    void write_line(const char* str, uint32_t foreground);

    /// Sets the cursor on the terminal
    void set_cursor(uint16_t x, uint16_t y);
};  // namespace Spark::Terminal