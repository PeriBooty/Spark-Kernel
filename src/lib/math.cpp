#include <lib/math.hpp>

size_t Math::round_up(size_t N, size_t S) {
    return ((N + S - 1) / S) * S;
}


size_t Math::round_down(size_t N, size_t S) {
    return (N / S) * S;
}


size_t Math::overlaps(size_t a, size_t as, size_t b, size_t bs) {
    return a >= b && a + as <= b + bs;
}

size_t Math::max(size_t x, size_t y) {
    return x > y ? x : y;
}