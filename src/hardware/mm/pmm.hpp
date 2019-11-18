#pragma once
#include <stddef.h>
#include <stdint.h>
#include <multiboot.hpp>

namespace Spark::Pmm {
    /// Initializes physical memory management
    void init(Multiboot::MemoryMap* mmap, size_t mmap_len);

    /// Allocates memory by given alignment and upper memory
    void* alloc(size_t count, size_t alignment, uintptr_t upper);

    /// Allocates memory
    void* alloc(size_t count);

    /// Frees memory
    void free(void* mem, size_t count);
};  // namespace Spark::Pmm