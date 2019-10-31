#include <hardware/mm/mm.hpp>
#include <hardware/mm/pmm.hpp>
#include <hardware/mm/vmm.hpp>

PageTableEntries virtual_to_entries(void *virt) {
    uintptr_t addr = (uintptr_t)virt;

    PageTableEntries off = {
        .pml4 = (addr & ((size_t)0x1ff << 39)) >> 39,
        .pdp = (addr & ((size_t)0x1ff << 30)) >> 30,
        .pd = (addr & ((size_t)0x1ff << 21)) >> 21,
        .pt = (addr & ((size_t)0x1ff << 12)) >> 12,
    };

    return off;
}

void *entries_to_virtual(PageTableEntries offs) {
    uintptr_t addr = 0;

    addr |= offs.pml4 << 39;
    addr |= offs.pdp << 30;
    addr |= offs.pd << 21;
    addr |= offs.pt << 12;

    return (void *)addr;
}

// TODO: multicore?
static PageTable *kernel_pml4;

void vmm_init() {
    kernel_pml4 = new_address_space();
    set_context(kernel_pml4);
}

static inline PageTable *get_or_alloc_ent(PageTable *tab, size_t off, int flags) {
    uint64_t ent_addr = tab->ents[off] & ADDR_MASK;
    if (!ent_addr) {
        ent_addr = tab->ents[off] = (uint64_t)alloc(1);
        if (!ent_addr) {
            return NULL;
        }
        tab->ents[off] |= flags | VirtualMemoryFlags::VMM_PRESENT;
        memset((void *)(ent_addr + VIRT_PHYS_BASE), 0, 4096);
    }

    return (PageTable *)(ent_addr + VIRT_PHYS_BASE);
}

static inline PageTable *get_or_null_ent(PageTable *tab, size_t off) {
    uint64_t ent_addr = tab->ents[off] & ADDR_MASK;
    if (!ent_addr) {
        return NULL;
    }

    return (PageTable *)(ent_addr + VIRT_PHYS_BASE);
}

int map_pages(PageTable *pml4, void *virt, void *phys, size_t count, int perms) {
    while (count--) {
        PageTableEntries offs = virtual_to_entries(virt);

        PageTable *pml4_virt = (PageTable *)((uint64_t)pml4 + VIRT_PHYS_BASE);
        PageTable *pdp_virt = get_or_alloc_ent(pml4_virt, offs.pml4, perms);
        PageTable *pd_virt = get_or_alloc_ent(pdp_virt, offs.pdp, perms);
        PageTable *pt_virt = get_or_alloc_ent(pd_virt, offs.pd, perms);
        pt_virt->ents[offs.pt] = (uint64_t)phys | perms | VirtualMemoryFlags::VMM_PRESENT;

        virt = (void *)((uintptr_t)virt + 0x1000);
        phys = (void *)((uintptr_t)phys + 0x1000);
    }

    return 1;
}

int unmap_pages(PageTable *pml4, void *virt, size_t count) {
    while (count--) {
        PageTableEntries offs = virtual_to_entries(virt);

        PageTable *pml4_virt = (PageTable *)((uint64_t)pml4 + VIRT_PHYS_BASE);
        PageTable *pdp_virt = get_or_null_ent(pml4_virt, offs.pml4);
        if (!pdp_virt) return 0;
        PageTable *pd_virt = get_or_null_ent(pdp_virt, offs.pdp);
        if (!pd_virt) return 0;
        PageTable *pt_virt = get_or_null_ent(pd_virt, offs.pd);
        if (!pt_virt) return 0;
        pt_virt->ents[offs.pt] = 0;

        virt = (void *)((uintptr_t)virt + 0x1000);
    }

    return 1;
}

int update_perms(PageTable *pml4, void *virt, size_t count, int perms) {
    while (count--) {
        PageTableEntries offs = virtual_to_entries(virt);

        PageTable *pml4_virt = (PageTable *)((uint64_t)pml4 + VIRT_PHYS_BASE);
        PageTable *pdp_virt = get_or_null_ent(pml4_virt, offs.pml4);
        if (!pdp_virt) return 0;
        PageTable *pd_virt = get_or_null_ent(pdp_virt, offs.pdp);
        if (!pd_virt) return 0;
        PageTable *pt_virt = get_or_null_ent(pd_virt, offs.pd);
        if (!pt_virt) return 0;
        pt_virt->ents[offs.pt] = (pt_virt->ents[offs.pt] & ADDR_MASK) | perms | VirtualMemoryFlags::VMM_PRESENT;

        virt = (void *)((uintptr_t)virt + 0x1000);
    }

    return 0;
}

int map_huge_pages(PageTable *pml4, void *virt, void *phys, size_t count, int perms) {
    while (count--) {
        PageTableEntries offs = virtual_to_entries(virt);

        PageTable *pml4_virt = (PageTable *)((uint64_t)pml4 + VIRT_PHYS_BASE);
        PageTable *pdp_virt = get_or_alloc_ent(pml4_virt, offs.pml4, perms);
        PageTable *pd_virt = get_or_alloc_ent(pdp_virt, offs.pdp, perms);
        pd_virt->ents[offs.pd] = (uint64_t)phys | perms | VirtualMemoryFlags::VMM_PRESENT | VirtualMemoryFlags::VMM_LARGE;

        virt = (void *)((uintptr_t)virt + 0x200000);
        phys = (void *)((uintptr_t)phys + 0x200000);
    }

    return 1;
}

int unmap_huge_pages(PageTable *pml4, void *virt, size_t count) {
    while (count--) {
        PageTableEntries offs = virtual_to_entries(virt);

        PageTable *pml4_virt = (PageTable *)((uint64_t)pml4 + VIRT_PHYS_BASE);
        PageTable *pdp_virt = get_or_null_ent(pml4_virt, offs.pml4);
        if (!pdp_virt) return 0;
        PageTable *pd_virt = get_or_null_ent(pdp_virt, offs.pdp);
        pd_virt->ents[offs.pd] = 0;

        virt = (void *)((uintptr_t)virt + 0x200000);
    }

    return 1;
}

int update_huge_perms(PageTable *pml4, void *virt, size_t count, int perms) {
    while (count--) {
        PageTableEntries offs = virtual_to_entries(virt);

        PageTable *pml4_virt = (PageTable *)((uint64_t)pml4 + VIRT_PHYS_BASE);
        PageTable *pdp_virt = get_or_null_ent(pml4_virt, offs.pml4);
        if (!pdp_virt) return 0;
        PageTable *pd_virt = get_or_null_ent(pdp_virt, offs.pdp);
        pd_virt->ents[offs.pd] = (pd_virt->ents[offs.pd] & ADDR_MASK) | perms | VirtualMemoryFlags::VMM_PRESENT | VirtualMemoryFlags::VMM_LARGE;

        virt = (void *)((uintptr_t)virt + 0x200000);
    }

    return 0;
}

uintptr_t get_entry(PageTable *pml4, void *virt) {
    if ((uintptr_t)virt >= 0xFFFFFFFF80000000)
        return (uintptr_t)virt - 0xFFFFFFFF80000000;
    if ((uintptr_t)virt >= 0xFFFF800000000000)
        return (uintptr_t)virt - 0xFFFF800000000000;

    PageTableEntries offs = virtual_to_entries(virt);

    PageTable *pml4_virt = (PageTable *)((uint64_t)pml4 + VIRT_PHYS_BASE);
    PageTable *pdp_virt = get_or_null_ent(pml4_virt, offs.pml4);
    if (!pdp_virt) return 0;
    PageTable *pd_virt = get_or_null_ent(pdp_virt, offs.pdp);
    if (!pd_virt) return 0;
    PageTable *pt_virt = get_or_null_ent(pd_virt, offs.pd);
    if (!pt_virt) return 0;
    return pt_virt->ents[offs.pt];
}

PageTable *new_address_space() {
    PageTable *new_pml4 = (PageTable *)alloc(1);
    memset((void *)((uintptr_t)new_pml4 + VIRT_PHYS_BASE), 0, 4096);

    map_huge_pages(new_pml4, (void *)0xFFFFFFFF80000000, NULL, 64, 3);
    map_huge_pages(new_pml4, (void *)0xFFFF800000000000, NULL, 512 * 4, 3);

    return new_pml4;
}

PageTable **get_ctx_ptr() {
    PageTable *ctx;
    asm volatile("mov %0, %%cr3"
                 : "=r"(*ctx->ents)::);
    return &ctx;
}

void save_context() {
    PageTable **ctx = get_ctx_ptr();
    *ctx = get_current_context();
}

PageTable *get_saved_context() {
    PageTable **ctx = get_ctx_ptr();
    return *ctx;
}

void restore_context() {
    PageTable **ctx = get_ctx_ptr();
    set_context(*ctx);
    *ctx = NULL;
}

void drop_context() {
    PageTable **ctx = get_ctx_ptr();
    *ctx = NULL;
}

void set_context(PageTable *ctx) {
    asm volatile("mov %%rax, %%cr3"
                 :
                 : "a"(ctx)
                 : "memory");
}

PageTable *get_current_context() {
    uintptr_t ctx = 0;
    asm volatile("mov %%cr3, %%rax"
                 : "=a"(ctx)
                 :
                 : "memory");
    return (PageTable *)ctx;
}

void update_mapping(void *ptr) {
    asm volatile("invlpg (%0)"
                 :
                 : "r"(ptr)
                 : "memory");
}

void ctx_memcpy(PageTable *dst_ctx, void *dst_addr, PageTable *src_ctx, void *src_addr, size_t size) {
    uintptr_t src_virt = 0x780000000000;
    uintptr_t dst_virt = 0x700000000000;
    uintptr_t dst = (uintptr_t)dst_addr & (~0xFFF);
    uintptr_t src = (uintptr_t)src_addr & (~0xFFF);
    for (size_t i = 0; i < size;
         i += 0x1000, src_virt += 0x1000, src += 0x1000,
                dst_virt += 0x1000, dst += 0x1000) {
        uintptr_t dst_phys = get_entry(dst_ctx, (void *)dst) & ADDR_MASK;
        uintptr_t src_phys = get_entry(src_ctx, (void *)src) & ADDR_MASK;

        map_pages(kernel_pml4, (void *)dst_virt, (void *)dst_phys, 1, VirtualMemoryFlags::VMM_WRITE);
        map_pages(kernel_pml4, (void *)src_virt, (void *)src_phys, 1, VirtualMemoryFlags::VMM_WRITE);
        update_mapping((void *)dst_virt);
        update_mapping((void *)src_virt);
    }

    memcpy((void *)(0x700000000000 + ((uintptr_t)dst_addr & 0xFFF)),
           (void *)(0x780000000000 + ((uintptr_t)src_addr & 0xFFF)), size);

    src_virt = 0x780000000000;
    dst_virt = 0x700000000000;
    for (size_t i = 0; i < size;
         i += 0x1000, src_virt += 0x1000, dst_virt += 0x1000) {
        unmap_pages(kernel_pml4, (void *)dst_virt, 1);
        unmap_pages(kernel_pml4, (void *)src_virt, 1);
        update_mapping((void *)dst_virt);
        update_mapping((void *)src_virt);
    }
}

int to_flags(int flags) {
    return ((flags & MemoryFlags::WRITE) ? VirtualMemoryFlags::VMM_WRITE : 0) | ((flags & MemoryFlags::USER) ? VirtualMemoryFlags::VMM_USER : 0) | ((flags & MemoryFlags::NO_CACHE) ? (VirtualMemoryFlags::VMM_NO_CACHE | VirtualMemoryFlags::VMM_WT) : 0);
}
int mm_map_kernel(int cpu, void *dst, void *src, size_t size, int flags) {
    return map_pages(kernel_pml4, dst, src, size, to_flags(flags));
}

int mm_unmap_kernel(int cpu, void *dst, size_t size) {
    return unmap_pages(kernel_pml4, dst, size);
}

uintptr_t mm_get_phys_kernel(int cpu, void *dst) {
    return get_entry(kernel_pml4, dst) & ADDR_MASK;
}

int mm_get_flags_kernel(int cpu, void *dst) {
    int arch_flags = get_entry(kernel_pml4, dst) & FLAG_MASK;
    int flags = flags ? (MemoryFlags::READ | MemoryFlags::EXECUTE) : 0;
    if (arch_flags & VirtualMemoryFlags::VMM_WRITE) flags |= MemoryFlags::WRITE;
    if (arch_flags & VirtualMemoryFlags::VMM_USER) flags |= MemoryFlags::USER;
    if (arch_flags & VirtualMemoryFlags::VMM_NO_CACHE) flags |= MemoryFlags::NO_CACHE;
    if (arch_flags & VirtualMemoryFlags::VMM_WT) flags |= MemoryFlags::NO_CACHE;
    return flags;
}

void *mm_get_ctx_kernel(int cpu) {
    return kernel_pml4;
}

int mm_store_context() {
    save_context();
    return 1;
}

int mm_switch_context(void *ctx) {
    set_context((PageTable *)ctx);
    return 1;
}

int mm_restore_context() {
    restore_context();
    return 1;
}

int mm_drop_context() {
    drop_context();
    return 1;
}

int mm_update_context_all(int cpu) {
    PageTable *ctx = get_current_context();
    set_context(ctx);
    return 1;
}

int mm_update_context_single(int cpu, void *dst) {
    update_mapping(dst);
    return 1;
}