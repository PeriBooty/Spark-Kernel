#pragma once
#include <stddef.h>
#include <stdint.h>
#include <multiboot.hpp>

namespace Spark::Pmm {
    /**
     * @brief Initializes physical memory management
     * @param mmap
     * @param mmap_len
     */
    void init(Multiboot::MemoryMap* mmap, size_t mmap_len);

    /**
     * @brief Allocates memory by given alignment and upper memory
     * @param count
     * @param alignment
     * @param upper
     * @return
     */
    void* alloc(size_t count, size_t alignment, uintptr_t upper);

    /**
     * @brief Allocates memory
     * @param count
     * @return
     */
    void* alloc(size_t count);

    /**
     * @brief Frees memory
     * @param mem
     * @param count
     */
    void free(void* mem, size_t count);
};  // namespace Spark::Pmm