#pragma once
#include <stddef.h>
#include <stdint.h>
#include <multiboot.hpp>

class Pmm {
private:
    /// The bitmap
    static uint64_t* bitmap;

    /// Total free pages
    static size_t free_pages;

    /// Total pages
    static size_t total_pages;

    /// Count of bitmaps
    static size_t bitmap_len;

    /// Read bitmap
    static bool bit_read(size_t idx);

    /// Write bitmap
    static void bit_write(size_t idx, int bit, size_t count);

    /// Tells you if a bitmap is free
    static bool bitmap_is_free(size_t idx, size_t count);

    /// Finds available memory on the top
    static uintptr_t find_available_memory_top(MultibootMemoryMap* mmap, size_t mmap_len);

public:
    /// Initializes physical memory management
    static void init(MultibootMemoryMap* mmap, size_t mmap_len);

    /// Allocates memory by given alignment and upper memory
    static void* alloc(size_t count, size_t alignment, uintptr_t upper);

    /// Allocates memory
    static void* alloc(size_t count);

    /// Frees memory
    static void free(void* mem, size_t count);
};