#pragma once
#include <stdint.h>

enum VGAColor {
    BLACK = 0,
    BLUE = 1,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHT_GRAY = 7,
    DARK_GRAY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    LIGHT_MAGENTA = 13,
    YELLOW = 14,
    WHITE = 15
};

class Display {
private:
    static uint16_t width;
    static uint16_t height;
    static uint16_t y;
    static uint16_t x;
    static uint64_t* display_buffer;
    static uint16_t prepare_char(const char c);
    static bool cursor_enabled;

public:
    static void set_cursor_pos(int x, int y);
    static void init(uint64_t fb, uint16_t width, uint16_t height);
    static bool is_cursor_enabled();
    static void toggle_cursor(bool enable);
    static void clear();
    static void handle_write_pos();
    static bool handle_special_characters(const char c);
    static void write(const char c);
    static void write(const char* str);
    static void write_line(const char c);
    static void write_line(const char* str);
    static void backspace();
    static VGAColor background;
    static VGAColor foreground;
};