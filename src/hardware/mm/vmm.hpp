#pragma once
#include <stddef.h>
#include <stdint.h>
constexpr inline size_t flag_mask = 0xFFF;
constexpr inline size_t address_mask = ~(flag_mask);

/// The virtual memory flags
enum VirtualMemoryFlags {
    VMM_PRESENT = 1 << 0,
    VMM_WRITE = 1 << 1,
    VMM_USER = 1 << 2,
    VMM_WT = 1 << 3,
    VMM_NO_CACHE = 1 << 4,
    VMM_DIRTY = 1 << 5,
    VMM_LARGE = 1 << 7,
};

/// The page table struct
struct PageTable {
    uint64_t ents[512];
};

/// The page table entries struct
struct PageTableEntries {
    size_t pml4;
    size_t pdp;
    size_t pd;
    size_t pt;
};

class VMM {
private:
    /// The kernel's PML4
    static PageTable* kernel_pml4;

    /// Gets or allocates a page table entry
    static inline PageTable* get_or_alloc_ent(PageTable* tab, size_t off, int flags);

    /// Gets or nulls a page table entry
    static inline PageTable* get_or_null_ent(PageTable* tab, size_t off);

    /// Updates the mapping
    static void update_mapping(void* ptr);

    /// Gives the saved context's pointer
    static PageTable* get_saved_context();

public:
    /// Converts virtual address to page table entries
    static PageTableEntries virtual_to_entries(void* virt);

    /// Converts page table entries to a virtual address
    static void* entries_to_virtual(PageTableEntries entries);

    /// Initializes 4-level paging
    static void init(bool is_pat_supported);

    /// Maps a virtual address
    static bool map_pages(PageTable* pml4, void* virt, void* phys, size_t count, int perms);

    /// Unmaps a virtual address
    static bool unmap_pages(PageTable* pml4, void* virt, size_t count);

    /// Updates a virtual address' permissions
    static bool update_perms(PageTable* pml4, void* virt, size_t count, int perms);

    /// Maps a huge memory page
    static bool map_huge_pages(PageTable* pml4, void* virt, void* phys, size_t count, int perms);

    /// Unmaps a huge memory page
    static bool unmap_huge_pages(PageTable* pml4, void* virt, size_t count);

    /// Updates a huge memory page's virtual address' permissions
    static bool update_huge_perms(PageTable* pml4, void* virt, size_t count, int perms);

    /// Gets physical address
    static uintptr_t get_entry(PageTable* pml4, void* virt);

    /// Creates a new address space
    static PageTable* new_address_space();

    /// Saves the current context
    static void save_context();

    /// Gives the saved context's pointer
    static PageTable** get_ctx_ptr();

    /// Restores the context
    static void restore_context();

    /// Drops the context
    static void drop_context();

    /// Sets the context
    static void set_context(PageTable* ctx);

    /// Gives the current context
    static PageTable* get_current_context();

    /// Converts physical memory flags to virtual memory flags
    static int to_flags(int flags);

    /// Memcpy but for paging
    static void ctx_memcpy(PageTable* dst_ctx, void* dst_addr, PageTable* src_ctx, void* src_addr, size_t size);
};