#pragma once
#include <stddef.h>
#include <stdint.h>
#define FLAG_MASK 0xFFF
#define ADDR_MASK ~(FLAG_MASK)

enum VirtualMemoryFlags {
    VMM_PRESENT = (1 << 0),
    VMM_WRITE = (1 << 1),
    VMM_USER = (1 << 2),
    VMM_WT = (1 << 3),
    VMM_NO_CACHE = (1 << 4),
    VMM_DIRTY = (1 << 5),
    VMM_LARGE = (1 << 7),
};

struct PageTable {
    uint64_t ents[512];
};

struct PageTableEntries {
    size_t pml4;
    size_t pdp;
    size_t pd;
    size_t pt;
};

PageTableEntries virtual_to_entries(void *virt);
void *entries_to_virtual(PageTableEntries entries);
void vmm_init();
int map_pages(PageTable *pml4, void *virt, void *phys, size_t count, int perms);
int unmap_pages(PageTable *pml4, void *virt, size_t count);
int update_perms(PageTable *pml4, void *virt, size_t count, int perms);
uintptr_t get_entry(PageTable *pml4, void *virt);
PageTable *new_address_space();
void save_context();
PageTable *get_saved_context();
void restore_context();
void drop_context();
void set_context(PageTable *ctx);
PageTable *get_current_context();
int to_flags(int);
void ctx_memcpy(PageTable *, void *, PageTable *, void *, size_t);