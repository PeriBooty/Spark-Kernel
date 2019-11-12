#pragma once
#include <stddef.h>
#include <stdint.h>
#include <multiboot.hpp>

/// Initializes physical memory management
void pmm_init(MultibootMemoryMap *mmap, size_t mmap_len);

/// Allocates memory by given alignment and upper memory
void *pmm_alloc(size_t count, size_t alignment, uintptr_t upper);

/// Allocates memory
void *pmm_alloc(size_t count);

/// Frees memory
void pmm_free(void *mem, size_t count);