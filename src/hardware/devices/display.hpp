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
    const static uint8_t width = 80;
    const static uint8_t height = 15;
    static uint16_t y;
    static uint16_t x;
    static uint16_t* display_buffer;
    static uint16_t prepare_char(const char);
    static bool cursor_enabled;

public:
    static void set_cursor_pos(int, int);
    static void init();
    static bool is_cursor_enabled();
    static void toggle_cursor(bool);
    static void clear();
    static void handle_write_pos();
    static bool handle_special_characters(const char);
    static void write(const char);
    static void write(const char*);
    static void write_line(const char);
    static void write_line(const char*);
    static void backspace();
    static VGAColor background;
    static VGAColor foreground;
};