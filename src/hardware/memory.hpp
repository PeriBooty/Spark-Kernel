#pragma once
#define CHECK_BIT(variable, position) ((variable) & (1 << (position)))
#include <stdint.h>

class PageTable {
public:
    uint64_t entry[512];
};

[[gnu::always_inline]] static inline uint64_t get_pml_index(int pml, uint64_t address) {
    return (pml >= 4 ? address >> 39 : address >> 12 + (pml - 1) * 9) & 0x1FF;
}

[[gnu::always_inline]] static inline uint64_t virtual_to_physical(uint64_t address) {
    return address - 0x100000000;
}