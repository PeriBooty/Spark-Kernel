#include <hardware/mm/mm.hpp>
#include <hardware/mm/pmm.hpp>
#include <hardware/mm/vmm.hpp>
#include <lib/lib.hpp>
#include <lib/spinlock.hpp>

static Spinlock mm_lock{};
static uintptr_t top = memory_base;


/// Allocates physical memory
void* malloc(size_t bytes) {
    mm_lock.lock();
    bytes = ((bytes + 7) / 8) * 8;
    bytes += 16;
    size_t pages = (bytes + page_size - 1) / page_size + 1;
    void* out = reinterpret_cast<void*>(top);

    for (size_t i = 0; i < pages; i++) {
        void* p = Pmm::alloc(1);
        if (!p) {
            mm_lock.release();
            return NULL;
        }
        Vmm::map_pages(Vmm::get_current_context(), reinterpret_cast<void*>(top), p, 1, Vmm::to_flags(MemoryFlags::READ | MemoryFlags::WRITE));
        top += page_size;
    }

    top += page_size;

    out = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(out) + (pages * page_size - bytes));

    static_cast<uint64_t*>(out)[0] = bytes - 16;
    static_cast<uint64_t*>(out)[1] = pages;

    mm_lock.release();

    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(out) + 16);
}

/// Safely allocates memory by zeroing it
void* calloc(size_t bytes, size_t elem) {
    void* out = malloc(bytes * elem);
    memset(out, 0, bytes * elem);
    return out;
}

/// Reallocates memory
void* realloc(void* old, size_t s) {
    void* newm = malloc(s);
    if (old) {
        mm_lock.lock();
        uint64_t size = *reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(old) - 16);
        mm_lock.release();
        memcpy(newm, old, size);
        free(old);
    }
    return newm;
}


/// Frees memory
int free(void* memory) {
    mm_lock.lock();
    size_t size = *reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(memory) - 16);
    size_t req_pages = *reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(memory) - 8);
    void* start = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(memory) & (~(page_size - 1)));

    size += 16;
    size_t pages = (size + page_size - 1) / page_size + 1;

    if (req_pages != pages) {
        return 0;
    }

    for (size_t i = 0; i < pages; i++) {
        void* curr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(start) + i * page_size);
        void* p = reinterpret_cast<void*>(Vmm::get_entry(Vmm::get_current_context(), curr));
        Vmm::unmap_pages(Vmm::get_current_context(), curr, 1);
        Pmm::free(p, 1);
    }
    mm_lock.release();
    return 1;
}


/// Fills memory with something
void* memset(void* s, int c, size_t n) {
    unsigned char* p = static_cast<unsigned char*>(s);
    unsigned char fill = static_cast<unsigned char>(c);
    while (n--)
        *p++ = fill;
    return s;
}


/// Copies memory
void* memcpy(void* dest, const void* src, size_t len) {
    char* d = static_cast<char*>(dest);
    const char* s = static_cast<const char*>(src);
    while (len--)
        *d++ = *s++;
    return dest;
}