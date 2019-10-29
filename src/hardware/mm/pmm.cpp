#include <stddef.h>
#include <stdint.h>
#include <hardware/mm/mm.hpp>
#include <lib/bit.hpp>
#include <multiboot.hpp>

static volatile uint32_t *memory_bitmap;
static volatile uint32_t initial_bitmap[] = { 0xffffff7f };
static volatile uint32_t *tmp_bitmap;
static size_t bitmap_entries = 32;
static size_t total_pages = 1;
static size_t free_pages = 1;
static size_t cur_ptr = BITMAP_BASE;

[[gnu::always_inline]] static inline int read_bitmap(size_t i) {
    i -= BITMAP_BASE;
    uint32_t bitmap_index = i / 32;
    uint32_t index_bit = i % 32;
    return test_bit(memory_bitmap[bitmap_index], index_bit);
}

[[gnu::always_inline]] static inline void set_bitmap(size_t i, size_t count) {
    i -= BITMAP_BASE;
    free_pages -= count;
    for (size_t j = i; j < i + count; j++)
        set_bit(memory_bitmap[j / 32], j % 32);
}

[[gnu::always_inline]] static inline void unset_bitmap(size_t i, size_t count) {
    i -= BITMAP_BASE;
    free_pages += count;
    size_t f = i + count;
    for (size_t j = i; j < f; j++)
        reset_bit(memory_bitmap[j / 32], j % 32);
}

static void *malloc_slow(size_t pg_count) {
    size_t pg_cnt = pg_count, i;
    for (i = BITMAP_BASE; i < BITMAP_BASE + bitmap_entries;) {
        if (!read_bitmap(i++)) {
            if (!--pg_cnt)
                goto found;
        } else
            pg_cnt = pg_count;
    }
    return NULL;

found:;
    size_t start = i - pg_count;
    set_bitmap(start, pg_count);
    return (void *)(start * PAGE_SIZE);
}

static void *malloc_fast(size_t pg_count) {
    size_t pg_cnt = pg_count;
    for (size_t i = 0; i < bitmap_entries; i++) {
        if (cur_ptr == BITMAP_BASE + bitmap_entries) {
            cur_ptr = BITMAP_BASE;
            pg_cnt = pg_count;
        }
        if (!read_bitmap(cur_ptr++)) {
            if (!--pg_cnt)
                goto found;
        } else
            pg_cnt = pg_count;
    }
    return NULL;

found:;
    size_t start = cur_ptr - pg_count;
    set_bitmap(start, pg_count);
    return (void *)(start * PAGE_SIZE);
}

void *(*malloc)(size_t pg_count) = malloc_slow;

void change_alloc_method() {
    malloc = malloc_fast;
}

void *calloc(size_t pg_count) {
    void *ptr = malloc(pg_count);
    if (!ptr) return NULL;
    uint64_t *pages = (uint64_t *)(ptr + PHYSICAL_MEM_MAPPING);
    for (size_t i = 0; i < (pg_count * PAGE_SIZE) / sizeof(uint64_t); i++)
        pages[i] = 0;
    return ptr;
}

void free(void *ptr, size_t pg_count) {
    size_t start = (size_t)ptr / PAGE_SIZE;
    unset_bitmap(start, pg_count);
}

void init_pmm(MultibootMemoryMap *mmap) {
    memory_bitmap = initial_bitmap;
    if (!(tmp_bitmap = (uint32_t *)calloc(BMREALLOC_STEP))) {
        for (;;)
            ;
    }

    tmp_bitmap = (uint32_t *)((size_t)tmp_bitmap + PHYSICAL_MEM_MAPPING);
    for (size_t i = 0; i < (BMREALLOC_STEP * PAGE_SIZE) / sizeof(uint32_t); i++)
        tmp_bitmap[i] = 0xFFFFFFFF;

    memory_bitmap = tmp_bitmap;
    bitmap_entries = ((PAGE_SIZE / sizeof(uint32_t)) * 32) * BMREALLOC_STEP;

    for (size_t i = 0; mmap[i].type; i++) {
        size_t aligned_base;
        if (mmap[i].addr % PAGE_SIZE)
            aligned_base = mmap[i].addr + (PAGE_SIZE - (mmap[i].addr % PAGE_SIZE));
        else
            aligned_base = mmap[i].addr;
        size_t aligned_length = (mmap[i].len / PAGE_SIZE) * PAGE_SIZE;
        if ((mmap[i].addr % PAGE_SIZE) && aligned_length) aligned_length -= PAGE_SIZE;

        for (size_t j = 0; j * PAGE_SIZE < aligned_length; j++) {
            size_t addr = aligned_base + j * PAGE_SIZE;

            size_t page = addr / PAGE_SIZE;

            if (addr < (MEMORY_BASE + PAGE_SIZE))
                continue;

            if (addr >= (MEMORY_BASE + bitmap_entries * PAGE_SIZE)) {
                size_t cur_bitmap_size_in_pages = ((bitmap_entries / 32) * sizeof(uint32_t)) / PAGE_SIZE;
                size_t new_bitmap_size_in_pages = cur_bitmap_size_in_pages + BMREALLOC_STEP;
                if (!(tmp_bitmap = (uint32_t *)calloc(new_bitmap_size_in_pages))) {
                    for (;;)
                        ;
                }
                tmp_bitmap = (uint32_t *)((size_t)tmp_bitmap + PHYSICAL_MEM_MAPPING);
                for (size_t i = 0; i < (cur_bitmap_size_in_pages * PAGE_SIZE) / sizeof(uint32_t); i++)
                    tmp_bitmap[i] = memory_bitmap[i];
                for (size_t i = (cur_bitmap_size_in_pages * PAGE_SIZE) / sizeof(uint32_t); i < (new_bitmap_size_in_pages * PAGE_SIZE) / sizeof(uint32_t); i++)
                    tmp_bitmap[i] = 0xffffffff;
                bitmap_entries += ((PAGE_SIZE / sizeof(uint32_t)) * 32) * BMREALLOC_STEP;
                uint32_t *old_bitmap = (uint32_t *)((size_t)memory_bitmap - PHYSICAL_MEM_MAPPING);
                memory_bitmap = tmp_bitmap;
                free(old_bitmap, cur_bitmap_size_in_pages);
            }

            if (mmap[i].type == 1) {
                total_pages++;
                unset_bitmap(page, 1);
            }
        }
    }
}