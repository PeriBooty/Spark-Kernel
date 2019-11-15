#pragma once
#include <stdint.h>

class Terminal {
private:
    static uint16_t x, y;

public:
    static bool handle_special_characters(const char c);
    static void write(const char* str, uint32_t foreground, uint32_t background);
    static void write(const char* str, uint32_t foreground);
    static void write(const char c, uint32_t foreground, uint32_t background);
    static void write(const char c, uint32_t foreground);
    static void write_line(const char* str, uint32_t foreground, uint32_t background);
    static void write_line(const char* str, uint32_t foreground);
};