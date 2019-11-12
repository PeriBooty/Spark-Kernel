#include <hardware/mm/mm.hpp>
#include <hardware/mm/pmm.hpp>
#include <lib/math.hpp>

uint64_t *pmm_bitmap = NULL;        // The bitmap
static size_t pmm_free_pages = 0;   // Total free pages
static size_t pmm_total_pages = 0;  // Total pages
size_t pmm_bitmap_len = 64;         // Count of bitmaps


/// Read bitmap
static bool pmm_bit_read(size_t idx) {
    size_t off = idx / 64, mask = (1 << (idx % 64));
    return (pmm_bitmap[off] & mask) == mask;
}

/// Write to bitmap
static void pmm_bit_write(size_t idx, int bit, size_t count) {
    for (; count; count--, idx++) {
        size_t off = idx / 64, mask = (1 << (idx % 64));
        if (bit)
            pmm_bitmap[off] |= mask;
        else
            pmm_bitmap[off] &= ~mask;
    }
}

/// Tells you if a bitmap is free
static bool pmm_bitmap_is_free(size_t idx, size_t count) {
    for (; count; idx++, count--) {
        if (pmm_bit_read(idx))
            return false;
    }
    return true;
}

/// Finds available memory on the top
static uintptr_t pmm_find_available_memory_top(MultibootMemoryMap *mmap, size_t mmap_len) {
    uintptr_t top = 0;
    for (size_t i = 0; i < mmap_len; i++) {
        if (mmap[i].type == MultibootMemoryState::AVAILABLE) {
            if (mmap[i].addr + mmap[i].len > top)
                top = mmap[i].addr + mmap[i].len;
        }
    }
    return top;
}

/// Initializes physical memory management
void pmm_init(MultibootMemoryMap *mmap, size_t mmap_len) {
    uintptr_t mem_top = pmm_find_available_memory_top(mmap, mmap_len);
    uint32_t mem_pages = (mem_top + page_size - 1) / page_size;
    pmm_bitmap = (uint64_t *)(memory_base + virtual_physical_base);
    pmm_bitmap_len = mem_pages;
    size_t bitmap_phys = (size_t)pmm_bitmap - virtual_physical_base;
    memset(pmm_bitmap, 0xFF, pmm_bitmap_len / 8);

    for (size_t i = 0; i < mmap_len; i++) {
        if (mmap[i].type == MultibootMemoryState::AVAILABLE) {
            uintptr_t start = Math::round_up(mmap[i].addr, page_size);
            size_t len = Math::round_down(mmap[i].len, page_size);
            size_t count = len / page_size;
            if (!len) continue;
            if (start + len < memory_base) continue;
            if (start < memory_base) {
                len -= memory_base - start;
                start = memory_base;
                count = len / page_size;
            }
            if (Math::overlaps(bitmap_phys, pmm_bitmap_len / 8, start, len)) {
                if (start < bitmap_phys)
                    pmm_free((void *)start, (start - bitmap_phys) / page_size);

                start = bitmap_phys + pmm_bitmap_len / 8;
                len -= pmm_bitmap_len / 8;
                count = len / page_size;
            }

            pmm_free((void *)start, count);
        }
    }

    pmm_total_pages = pmm_free_pages;
    pmm_bit_write(bitmap_phys / page_size, 1, (pmm_bitmap_len / 8 + page_size - 1) / page_size);
    pmm_alloc((pmm_bitmap_len / 8 + page_size - 1) / page_size);
}

/// Allocates memory by given alignment and upper memory
void *pmm_alloc(size_t count, size_t alignment, uintptr_t upper) {
    size_t idx = memory_base / page_size, max_idx = 0;

    if (!upper)
        max_idx = pmm_bitmap_len;
    else
        max_idx = pmm_bitmap_len < (upper / page_size) ? pmm_bitmap_len : (upper / page_size);

    while (idx < max_idx) {
        if (!pmm_bitmap_is_free(idx, count)) {
            idx += alignment;
            continue;
        }
        pmm_bit_write(idx, 1, count);
        if (pmm_total_pages)
            pmm_free_pages -= count;
        return (void *)(idx * page_size);
    }

    return NULL;
}

/// Allocates memory
void *pmm_alloc(size_t count) {
    return pmm_alloc(count, 1, 0);
}

/// Frees memory
void pmm_free(void *mem, size_t count) {
    size_t idx = (size_t)mem / page_size;
    pmm_bit_write(idx, 0, count);
    pmm_free_pages += count;
}