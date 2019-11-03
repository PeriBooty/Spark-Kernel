#pragma once
#include <stddef.h>
#include <stdint.h>
#define KERNEL_VMA 0xFFFF800000000000UL
#define VIRT_PHYS_BASE 0xFFFF800000000000UL
#define PAGE_SIZE 0x1000
#define MEMORY_BASE 0x2000000
#define HEAP_BASE 0x200000000

enum MemoryFlags {
    READ = 0x01,
    WRITE = 0x02,
    EXECUTE = 0x04,
    USER = 0x08,
    NO_CACHE = 0x10,
};

void *malloc(size_t blocks);
void *calloc(size_t blocks);
int free(void *memory);
void *memset(void *s, int c, size_t n);
void *memcpy (void *dest, const void *src, size_t len);
int mm_map_kernel(void *dst, void *src, size_t size, int flags);
int mm_unmap_kernel(void *dst, size_t size);
uintptr_t mm_get_phys_kernel(void *dst);
int mm_get_flags_kernel(void *dst);