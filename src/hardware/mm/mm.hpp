#pragma once
#include <stddef.h>
#include <stdint.h>
constexpr inline size_t virtual_kernel_base = 0xFFFF800000000000UL;
constexpr inline size_t virtual_physical_base = 0xFFFF800000000000UL;
constexpr inline size_t page_size = 0x1000;
constexpr inline size_t memory_base = 0x2000000;
constexpr inline size_t heap_base = 0x200000000;

/// Memory flags
enum MemoryFlags {
    READ = 0x01,
    WRITE = 0x02,
    EXECUTE = 0x04,
    USER = 0x08,
    NO_CACHE = 0x10,
};

/// Allocates physical memory
void *malloc(size_t blocks);

/// Safely allocates memory by zeroing it
void *calloc(size_t blocks);

/// Frees memory
int free(void *memory);

/// Fills memory with something
void *memset(void *s, int c, size_t n);

/// Copies memory
void *memcpy(void *dest, const void *src, size_t len);