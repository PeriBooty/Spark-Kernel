#include <stddef.h>
#include <stdint.h>
#include <lib/lib.hpp>
#include <lib/spinlock.hpp>
#include <hardware/mm/mm.hpp>
#include <hardware/mm/pmm.hpp>

static Spinlock mm_lock = { 0 };
static uintptr_t top = MEMORY_BASE;

void *malloc(size_t bytes) {
    spinlock_lock(&mm_lock);
    bytes = ((bytes + 7) / 8) * 8;
    bytes += 16;
    size_t pages = (bytes + PAGE_SIZE - 1) / PAGE_SIZE + 1;
    void *out = (void *)top;

    for (size_t i = 0; i < pages; i++) {
        void *p = malloc(1);
        if (!p) {
            spinlock_release(&mm_lock);
            return NULL;
        }
        mm_map_kernel(-1, (void *)top, p, 1, MemoryFlags::READ | MemoryFlags::WRITE);
        top += PAGE_SIZE;
    }

    top += PAGE_SIZE;

    out = (void *)((uintptr_t)out + (pages * PAGE_SIZE - bytes));

    ((uint64_t *)out)[0] = bytes - 16;
    ((uint64_t *)out)[1] = pages;

    spinlock_release(&mm_lock);

    return (void *)((uintptr_t)out + 16);
}

void *calloc(size_t bytes, size_t elem) {
    void *out = malloc(bytes * elem);
    memset(out, 0, bytes * elem);
    return out;
}

void *realloc(void *old, size_t s) {
    void *newm = malloc(s);
    if (old) {
        spinlock_lock(&mm_lock);
        uint64_t size = *(uint64_t *)((uintptr_t)old - 16);
        spinlock_release(&mm_lock);
        memcpy(newm, old, size);
        free(old);
    }
    return newm;
}

int free(void *memory) {
    spinlock_lock(&mm_lock);
    size_t size = *(uint64_t *)((uintptr_t)memory - 16);
    size_t req_pages = *(uint64_t *)((uintptr_t)memory - 8);
    void *start = (void *)((uintptr_t)memory & (~(PAGE_SIZE - 1)));

    size += 16;
    size_t pages = (size + PAGE_SIZE - 1) / PAGE_SIZE + 1;

    if (req_pages != pages) {
        return 0;
    }

    for (size_t i = 0; i < pages; i++) {
        void *curr = (void *)((uintptr_t)start + i * PAGE_SIZE);
        void *p = (void *)mm_get_phys_kernel(-1, curr);
        mm_unmap_kernel(-1, curr, 1);
        pmm_free(p, 1);
    }
    spinlock_release(&mm_lock);
    return 1;
}

void *memset(void *s, int c, size_t n) {
    unsigned char *p = (unsigned char *)s;
    while (n--)
        *p++ = (unsigned char)c;
    return s;
}

void *memcpy(void *dest, const void *src, size_t len) {
    char *d = (char *)dest;
    const char *s = (const char *)src;
    while (len--)
        *d++ = *s++;
    return dest;
}