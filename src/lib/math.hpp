#pragma once
#include <stddef.h>

namespace Spark::Math {
    /// Rounds up two numbers
    size_t round_up(size_t N, size_t S);

    /// Rounds down two numbers
    size_t round_down(size_t N, size_t S);

    /// Checks if numbers overlap
    size_t overlaps(size_t a, size_t as, size_t b, size_t bs);

    /// Gets maximum value of two numbers
    size_t max(size_t x, size_t y);
};  // namespace Spark::Math