#pragma once
#include <stddef.h>

namespace Spark::Math {
    /**
     * @brief Rounds up two numbers
     * 
     * @param n 
     * @param s 
     * @return size_t 
     */
    size_t round_up(size_t n, size_t s);

    /**
     * @brief Rounds down two numbers
     * 
     * @param n 
     * @param s 
     * @return size_t 
     */
    size_t round_down(size_t n, size_t s);

    /**
     * @brief Checks if numbers overlap
     * 
     * @param a 
     * @param as 
     * @param b 
     * @param bs 
     * @return size_t 
     */
    size_t overlaps(size_t a, size_t as, size_t b, size_t bs);

    /**
     * @brief Gets maximum value of two numbers
     * 
     * @param x 
     * @param y 
     * @return size_t 
     */
    size_t max(size_t x, size_t y);
};  // namespace Spark::Math