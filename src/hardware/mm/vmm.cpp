#include <hardware/cpu/cpu.hpp>
#include <hardware/mm/mm.hpp>
#include <hardware/mm/pmm.hpp>
#include <hardware/mm/vmm.hpp>
#include <lib/math.hpp>

/// Converts virtual address to page table entries
PageTableEntries virtual_to_entries(void *virt) {
    uintptr_t addr = (uintptr_t)virt;

    PageTableEntries off = {
        .pml4 = (addr >> 39) & 0x1ff,
        .pdp = (addr >> 30) & 0x1ff,
        .pd = (addr >> 21) & 0x1ff,
        .pt = (addr >> 12) & 0x1ff,
    };

    return off;
}

/// Converts page table entries to a virtual address
void *entries_to_virtual(PageTableEntries offs) {
    uintptr_t addr = 0;

    addr |= offs.pml4 << 39;
    addr |= offs.pdp << 30;
    addr |= offs.pd << 21;
    addr |= offs.pt << 12;

    return (void *)addr;
}

// TODO: multicore?
static PageTable *kernel_pml4;  // The kernel's PML4

/// Initializes 4-level paging
void vmm_init(bool is_pat_supported) {
    kernel_pml4 = new_address_space();
    vmm_set_context(kernel_pml4);
    if (is_pat_supported)
        cpu_set_msr(0x277, 0x00'00'01'00'00'00'04'06);
}

/// Gets or allocates a page table entry
static inline PageTable *get_or_alloc_ent(PageTable *tab, size_t off, int flags) {
    uint64_t ent_addr = tab->ents[off] & address_mask;
    if (!ent_addr) {
        ent_addr = tab->ents[off] = (uint64_t)pmm_alloc(1);
        if (!ent_addr)
            return NULL;
        tab->ents[off] |= flags | VirtualMemoryFlags::VMM_PRESENT;
        memset((void *)(ent_addr + virtual_physical_base), 0, 4096);
    }

    return (PageTable *)(ent_addr + virtual_physical_base);
}

/// Gets or nulls a page table entry
static inline PageTable *get_or_null_ent(PageTable *tab, size_t off) {
    uint64_t ent_addr = tab->ents[off] & address_mask;
    if (!ent_addr)
        return NULL;

    return (PageTable *)(ent_addr + virtual_physical_base);
}

/// Maps a virtual address
bool map_pages(PageTable *pml4, void *virt, void *phys, size_t count, int perms) {
    while (count--) {
        PageTableEntries offs = virtual_to_entries(virt);

        PageTable *pml4_virt = (PageTable *)((uint64_t)pml4 + virtual_physical_base);
        PageTable *pdp_virt = get_or_alloc_ent(pml4_virt, offs.pml4, perms);
        PageTable *pd_virt = get_or_alloc_ent(pdp_virt, offs.pdp, perms);
        PageTable *pt_virt = get_or_alloc_ent(pd_virt, offs.pd, perms);
        pt_virt->ents[offs.pt] = (uint64_t)phys | perms | VirtualMemoryFlags::VMM_PRESENT;

        virt = (void *)((uintptr_t)virt + 0x1000);
        phys = (void *)((uintptr_t)phys + 0x1000);
    }

    return true;
}

/// Unmaps a virtual address
bool unmap_pages(PageTable *pml4, void *virt, size_t count) {
    while (count--) {
        PageTableEntries offs = virtual_to_entries(virt);

        PageTable *pml4_virt = (PageTable *)((uint64_t)pml4 + virtual_physical_base);
        PageTable *pdp_virt = get_or_null_ent(pml4_virt, offs.pml4);
        if (!pdp_virt) return false;
        PageTable *pd_virt = get_or_null_ent(pdp_virt, offs.pdp);
        if (!pd_virt) return false;
        PageTable *pt_virt = get_or_null_ent(pd_virt, offs.pd);
        if (!pt_virt) return false;
        pt_virt->ents[offs.pt] = 0;

        virt = (void *)((uintptr_t)virt + 0x1000);
    }

    return true;
}

/// Updates a virtual address' permissions
bool vmm_update_perms(PageTable *pml4, void *virt, size_t count, int perms) {
    while (count--) {
        PageTableEntries offs = virtual_to_entries(virt);

        PageTable *pml4_virt = (PageTable *)((uint64_t)pml4 + virtual_physical_base);
        PageTable *pdp_virt = get_or_null_ent(pml4_virt, offs.pml4);
        if (!pdp_virt) return false;
        PageTable *pd_virt = get_or_null_ent(pdp_virt, offs.pdp);
        if (!pd_virt) return false;
        PageTable *pt_virt = get_or_null_ent(pd_virt, offs.pd);
        if (!pt_virt) return false;
        pt_virt->ents[offs.pt] = (pt_virt->ents[offs.pt] & address_mask) | perms | VirtualMemoryFlags::VMM_PRESENT;

        virt = (void *)((uintptr_t)virt + 0x1000);
    }

    return true;
}

/// Maps a huge memory page
bool map_huge_pages(PageTable *pml4, void *virt, void *phys, size_t count, int perms) {
    while (count--) {
        PageTableEntries offs = virtual_to_entries(virt);

        PageTable *pml4_virt = (PageTable *)((uint64_t)pml4 + virtual_physical_base);
        PageTable *pdp_virt = get_or_alloc_ent(pml4_virt, offs.pml4, perms);
        PageTable *pd_virt = get_or_alloc_ent(pdp_virt, offs.pdp, perms);
        pd_virt->ents[offs.pd] = (uint64_t)phys | perms | VirtualMemoryFlags::VMM_PRESENT | VirtualMemoryFlags::VMM_LARGE;

        virt = (void *)((uintptr_t)virt + 0x200000);
        phys = (void *)((uintptr_t)phys + 0x200000);
    }

    return true;
}

/// Unmaps a huge memory page
bool unmap_huge_pages(PageTable *pml4, void *virt, size_t count) {
    while (count--) {
        PageTableEntries offs = virtual_to_entries(virt);

        PageTable *pml4_virt = (PageTable *)((uint64_t)pml4 + virtual_physical_base);
        PageTable *pdp_virt = get_or_null_ent(pml4_virt, offs.pml4);
        if (!pdp_virt) return false;
        PageTable *pd_virt = get_or_null_ent(pdp_virt, offs.pdp);
        pd_virt->ents[offs.pd] = 0;

        virt = (void *)((uintptr_t)virt + 0x200000);
    }

    return true;
}

/// Updates a huge memory page's virtual address' permissions
bool update_huge_perms(PageTable *pml4, void *virt, size_t count, int perms) {
    while (count--) {
        PageTableEntries offs = virtual_to_entries(virt);

        PageTable *pml4_virt = (PageTable *)((uint64_t)pml4 + virtual_physical_base);
        PageTable *pdp_virt = get_or_null_ent(pml4_virt, offs.pml4);
        if (!pdp_virt) return false;
        PageTable *pd_virt = get_or_null_ent(pdp_virt, offs.pdp);
        pd_virt->ents[offs.pd] = (pd_virt->ents[offs.pd] & address_mask) | perms | VirtualMemoryFlags::VMM_PRESENT | VirtualMemoryFlags::VMM_LARGE;

        virt = (void *)((uintptr_t)virt + 0x200000);
    }

    return true;
}

/// Gets physical address
uintptr_t vmm_get_entry(PageTable *pml4, void *virt) {
    if ((uintptr_t)virt >= 0xFFFFFFFF80000000)
        return (uintptr_t)virt - 0xFFFFFFFF80000000;
    if ((uintptr_t)virt >= 0xFFFF800000000000)
        return (uintptr_t)virt - 0xFFFF800000000000;

    PageTableEntries offs = virtual_to_entries(virt);

    PageTable *pml4_virt = (PageTable *)((uint64_t)pml4 + virtual_physical_base);
    PageTable *pdp_virt = get_or_null_ent(pml4_virt, offs.pml4);
    if (!pdp_virt) return 0;
    PageTable *pd_virt = get_or_null_ent(pdp_virt, offs.pdp);
    if (!pd_virt) return 0;
    PageTable *pt_virt = get_or_null_ent(pd_virt, offs.pd);
    if (!pt_virt) return 0;
    return pt_virt->ents[offs.pt];
}

/// Creates a new address space
PageTable *new_address_space() {
    PageTable *new_pml4 = (PageTable *)pmm_alloc(1);
    memset((void *)((uintptr_t)new_pml4 + virtual_physical_base), 0, 4096);

    map_huge_pages(new_pml4, (void *)0xFFFFFFFF80000000, NULL, 64, 3);
    map_huge_pages(new_pml4, (void *)0xFFFF800000000000, NULL, 512 * 4, 3);

    return new_pml4;
}

/// Gives the saved context's pointer
PageTable **vmm_get_ctx_ptr() {
    return (PageTable **)vmm_get_current_context();  // I don't know what I did but DON'T TOUCH IT!!
}

/// Saves the current context
void vmm_save_context() {
    PageTable **ctx = vmm_get_ctx_ptr();
    *ctx = vmm_get_current_context();
}

/// Gives the saved context's pointer
PageTable *vmm_get_saved_context() {
    PageTable **ctx = vmm_get_ctx_ptr();
    return *ctx;
}

/// Restores the context
void vmm_restore_context() {
    PageTable **ctx = vmm_get_ctx_ptr();
    vmm_set_context(*ctx);
    *ctx = NULL;
}

/// Drops the context
void vmm_drop_context() {
    PageTable **ctx = vmm_get_ctx_ptr();
    *ctx = NULL;
}

/// Sets the context
void vmm_set_context(PageTable *ctx) {
    asm volatile("mov %%rax, %%cr3"
                 :
                 : "a"(ctx)
                 : "memory");
}

/// Gives the current context
PageTable *vmm_get_current_context() {
    uintptr_t ctx = 0;
    asm volatile("mov %%cr3, %%rax"
                 : "=a"(ctx)
                 :
                 : "memory");
    return (PageTable *)ctx;
}

/// Updates the mapping
void vmm_update_mapping(void *ptr) {
    asm volatile("invlpg (%0)"
                 :
                 : "r"(ptr)
                 : "memory");
}

/// Copies the context
void vmm_ctx_memcpy(PageTable *dst_ctx, void *dst_addr, PageTable *src_ctx, void *src_addr, size_t size) {
    uintptr_t src_virt = 0x780000000000;
    uintptr_t dst_virt = 0x700000000000;
    uintptr_t dst = (uintptr_t)dst_addr & (~0xFFF);
    uintptr_t src = (uintptr_t)src_addr & (~0xFFF);
    size_t map_size = size + Math::max((uintptr_t)dst_addr & 0xFFF, (uintptr_t)src_addr & 0xFFF);
    for (size_t i = 0; i < map_size;
         i += 0x1000, src_virt += 0x1000, src += 0x1000,
                dst_virt += 0x1000, dst += 0x1000) {
        uintptr_t dst_phys = vmm_get_entry(dst_ctx, (void *)dst) & address_mask;
        uintptr_t src_phys = vmm_get_entry(src_ctx, (void *)src) & address_mask;

        map_pages(kernel_pml4, (void *)dst_virt, (void *)dst_phys, 1, VirtualMemoryFlags::VMM_WRITE);
        map_pages(kernel_pml4, (void *)src_virt, (void *)src_phys, 1, VirtualMemoryFlags::VMM_WRITE);
        vmm_update_mapping((void *)dst_virt);
        vmm_update_mapping((void *)src_virt);
    }

    memcpy((void *)(0x700000000000 + ((uintptr_t)dst_addr & 0xFFF)),
           (void *)(0x780000000000 + ((uintptr_t)src_addr & 0xFFF)), size);

    src_virt = 0x780000000000;
    dst_virt = 0x700000000000;
    for (size_t i = 0; i < size;
         i += 0x1000, src_virt += 0x1000, dst_virt += 0x1000) {
        unmap_pages(kernel_pml4, (void *)dst_virt, 1);
        unmap_pages(kernel_pml4, (void *)src_virt, 1);
        vmm_update_mapping((void *)dst_virt);
        vmm_update_mapping((void *)src_virt);
    }
}

/// Converts flags int to flags
int vmm_to_flags(int flags) {
    return ((flags & MemoryFlags::WRITE) ? VirtualMemoryFlags::VMM_WRITE : 0) | ((flags & MemoryFlags::USER) ? VirtualMemoryFlags::VMM_USER : 0) | ((flags & MemoryFlags::NO_CACHE) ? (VirtualMemoryFlags::VMM_NO_CACHE | VirtualMemoryFlags::VMM_WT) : 0);
}

/// Maps kernel memory
bool mm_map_kernel(void *dst, void *src, size_t size, int flags) {
    return map_pages(kernel_pml4, dst, src, size, vmm_to_flags(flags));
}

// Unmaps kernel memory
bool mm_unmap_kernel(void *dst, size_t size) {
    return unmap_pages(kernel_pml4, dst, size);
}

/// Gets kernel physical memory
uintptr_t mm_get_phys_kernel(void *dst) {
    return vmm_get_entry(kernel_pml4, dst) & address_mask;
}

/// Gets flags from kernel memory
int mm_get_flags_kernel(void *dst) {
    int arch_flags = vmm_get_entry(kernel_pml4, dst) & flag_mask;
    int flags = arch_flags ? (MemoryFlags::READ | MemoryFlags::EXECUTE) : 0;
    if (arch_flags & VirtualMemoryFlags::VMM_WRITE) flags |= MemoryFlags::WRITE;
    if (arch_flags & VirtualMemoryFlags::VMM_USER) flags |= MemoryFlags::USER;
    if (arch_flags & VirtualMemoryFlags::VMM_NO_CACHE) flags |= MemoryFlags::NO_CACHE;
    if (arch_flags & VirtualMemoryFlags::VMM_WT) flags |= MemoryFlags::NO_CACHE;
    return flags;
}

/// Gets the kernel context
void *mm_get_ctx_kernel() {
    return kernel_pml4;
}

/// Stores the context
bool mm_store_context() {
    vmm_save_context();
    return true;
}

/// Switches the context
bool mm_switch_context(void *ctx) {
    vmm_set_context((PageTable *)ctx);
    return true;
}

/// Restores the context
int mm_restore_context() {
    vmm_restore_context();
    return true;
}

/// Drops the context
bool mm_drop_context() {
    vmm_drop_context();
    return true;
}

/// Updates the context for all CPUs
bool mm_update_context_all() {
    PageTable *ctx = vmm_get_current_context();
    vmm_set_context(ctx);
    return true;
}

/// Updates the context for a single CPU
bool mm_update_context_single(void *dst) {
    vmm_update_mapping(dst);
    return true;
}