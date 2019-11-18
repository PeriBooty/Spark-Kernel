#pragma once
#include <stddef.h>
#include <stdint.h>

namespace Spark::Vmm {
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

    /// Converts virtual address to page table entries
    PageTableEntries virtual_to_entries(void* virt);

    /// Converts page table entries to a virtual address
    void* entries_to_virtual(PageTableEntries entries);

    /// Initializes 4-level paging
    void init(bool is_pat_supported);

    /// Maps a virtual address
    bool map_pages(PageTable* pml4, void* virt, void* phys, size_t count, int perms);

    /// Unmaps a virtual address
    bool unmap_pages(PageTable* pml4, void* virt, size_t count);

    /// Updates a virtual address' permissions
    bool update_perms(PageTable* pml4, void* virt, size_t count, int perms);

    /// Maps a huge memory page
    bool map_huge_pages(PageTable* pml4, void* virt, void* phys, size_t count, int perms);

    /// Unmaps a huge memory page
    bool unmap_huge_pages(PageTable* pml4, void* virt, size_t count);

    /// Updates a huge memory page's virtual address' permissions
    bool update_huge_perms(PageTable* pml4, void* virt, size_t count, int perms);

    /// Gets physical address
    uintptr_t get_entry(PageTable* pml4, void* virt);

    /// Creates a new address space
    PageTable* new_address_space();

    /// Saves the current context
    void save_context();

    /// Gives the saved context's pointer
    PageTable** get_ctx_ptr();

    /// Restores the context
    void restore_context();

    /// Drops the context
    void drop_context();

    /// Sets the context
    void set_context(PageTable* ctx);

    /// Gives the current context
    PageTable* get_current_context();

    /// Converts physical memory flags to virtual memory flags
    int to_flags(int flags);

    /// Memcpy but for paging
    void ctx_memcpy(PageTable* dst_ctx, void* dst_addr, PageTable* src_ctx, void* src_addr, size_t size);
};  // namespace Spark::Vmm