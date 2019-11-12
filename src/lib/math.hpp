#pragma once
#include <stddef.h>

class Math {
public:
    static size_t round_up(size_t N, size_t S);
    static size_t round_down(size_t N, size_t S);
    static size_t overlaps(size_t a, size_t as, size_t b, size_t bs);
    static size_t max(size_t x, size_t y);
};