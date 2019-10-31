#pragma once
#define test_bit(base, offset) (base & (1ull << offset))
#define set_bit(base, offset) (base |= (1ull << offset))
#define reset_bit(base, offset) (base &= ~(1ull << offset))