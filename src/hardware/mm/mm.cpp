#include <hardware/mm/mm.hpp>
#include <hardware/mm/pmm.hpp>
#include <hardware/mm/vmm.hpp>
#include <lib/lib.hpp>
#include <lib/spinlock.hpp>

static Spinlock mm_lock = { 0 };
static uintptr_t top = memory_base;


/// Allocates physical memory
void *mm_alloc(size_t bytes) {
    spinlock_lock(&mm_lock);
    bytes = ((bytes + 7) / 8) * 8;
    bytes += 16;
    size_t pages = (bytes + page_size - 1) / page_size + 1;
    void *out = (void *)top;

    for (size_t i = 0; i < pages; i++) {
        void *p = mm_alloc(1);
        if (!p) {
            spinlock_release(&mm_lock);
            return NULL;
        }
        mm_map_kernel((void *)top, p, 1, MemoryFlags::READ | MemoryFlags::WRITE);
        top += page_size;
    }

    top += page_size;

    out = (void *)((uintptr_t)out + (pages * page_size - bytes));

    ((uint64_t *)out)[0] = bytes - 16;
    ((uint64_t *)out)[1] = pages;

    spinlock_release(&mm_lock);

    return (void *)((uintptr_t)out + 16);
}

/// Safely allocates memory by zeroing it
void *mm_calloc(size_t bytes, size_t elem) {
    void *out = mm_alloc(bytes * elem);
    memset(out, 0, bytes * elem);
    return out;
}

/// Reallocates memory
void *mm_realloc(void *old, size_t s) {
    void *newm = mm_alloc(s);
    if (old) {
        spinlock_lock(&mm_lock);
        uint64_t size = *(uint64_t *)((uintptr_t)old - 16);
        spinlock_release(&mm_lock);
        memcpy(newm, old, size);
        mm_free(old);
    }
    return newm;
}


/// Frees memory
int mm_free(void *memory) {
    spinlock_lock(&mm_lock);
    size_t size = *(uint64_t *)((uintptr_t)memory - 16);
    size_t req_pages = *(uint64_t *)((uintptr_t)memory - 8);
    void *start = (void *)((uintptr_t)memory & (~(page_size - 1)));

    size += 16;
    size_t pages = (size + page_size - 1) / page_size + 1;

    if (req_pages != pages) {
        return 0;
    }

    for (size_t i = 0; i < pages; i++) {
        void *curr = (void *)((uintptr_t)start + i * page_size);
        void *p = (void *)mm_get_phys_kernel(curr);
        mm_unmap_kernel(curr, 1);
        pmm_free(p, 1);
    }
    spinlock_release(&mm_lock);
    return 1;
}


/// Fills memory with something
void *memset(void *s, int c, size_t n) {
    unsigned char *p = (unsigned char *)s;
    while (n--)
        *p++ = (unsigned char)c;
    return s;
}


/// Copies memory
void *memcpy(void *dest, const void *src, size_t len) {
    char *d = (char *)dest;
    const char *s = (const char *)src;
    while (len--)
        *d++ = *s++;
    return dest;
}