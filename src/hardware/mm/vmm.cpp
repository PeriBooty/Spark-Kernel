#include <hardware/cpu/cpu.hpp>
#include <hardware/mm/mm.hpp>
#include <hardware/mm/pmm.hpp>
#include <hardware/mm/vmm.hpp>
#include <lib/math.hpp>

PageTable* Vmm::kernel_pml4 = 0;

/// Converts virtual address to page table entries
PageTableEntries Vmm::virtual_to_entries(void* virt) {
    uintptr_t addr = reinterpret_cast<uintptr_t>(virt);

    PageTableEntries off = {
        .pml4 = (addr >> 39) & 0x1ff,
        .pdp = (addr >> 30) & 0x1ff,
        .pd = (addr >> 21) & 0x1ff,
        .pt = (addr >> 12) & 0x1ff,
    };

    return off;
}

/// Converts page table entries to a virtual address
void* Vmm::entries_to_virtual(PageTableEntries offs) {
    uintptr_t addr = 0;

    addr |= offs.pml4 << 39;
    addr |= offs.pdp << 30;
    addr |= offs.pd << 21;
    addr |= offs.pt << 12;

    return reinterpret_cast<void*>(addr);
}

/// Initializes 4-level paging
void Vmm::init(bool is_pat_supported) {
    kernel_pml4 = new_address_space();
    set_context(kernel_pml4);
    if (is_pat_supported)
        Cpu::set_msr(0x277, 0x00'00'01'00'00'00'04'06);
}

/// Gets or allocates a page table entry
inline PageTable* Vmm::get_or_alloc_ent(PageTable* tab, size_t off, int flags) {
    uint64_t ent_addr = tab->ents[off] & address_mask;
    if (!ent_addr) {
        ent_addr = tab->ents[off] = reinterpret_cast<uint64_t>(Pmm::alloc(1));
        if (!ent_addr)
            return NULL;
        tab->ents[off] |= flags | VirtualMemoryFlags::VMM_PRESENT;
        memset(reinterpret_cast<void*>(ent_addr + virtual_physical_base), 0, 4096);
    }

    return reinterpret_cast<PageTable*>(ent_addr + virtual_physical_base);
}

/// Gets or nulls a page table entry
inline PageTable* Vmm::get_or_null_ent(PageTable* tab, size_t off) {
    uint64_t ent_addr = tab->ents[off] & address_mask;
    if (!ent_addr)
        return NULL;

    return reinterpret_cast<PageTable*>(ent_addr + virtual_physical_base);
}

/// Maps a virtual address
bool Vmm::map_pages(PageTable* pml4, void* virt, void* phys, size_t count, int perms) {
    while (count--) {
        PageTableEntries offs = Vmm::virtual_to_entries(virt);

        PageTable* pml4_virt = reinterpret_cast<PageTable*>(reinterpret_cast<uint64_t>(pml4) + virtual_physical_base);
        PageTable* pdp_virt = get_or_alloc_ent(pml4_virt, offs.pml4, perms);
        PageTable* pd_virt = get_or_alloc_ent(pdp_virt, offs.pdp, perms);
        PageTable* pt_virt = get_or_alloc_ent(pd_virt, offs.pd, perms);
        pt_virt->ents[offs.pt] = reinterpret_cast<uint64_t>(phys) | perms | VirtualMemoryFlags::VMM_PRESENT;

        virt = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(virt) + 0x1000);
        phys = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(phys) + 0x1000);
    }

    return true;
}

/// Unmaps a virtual address
bool Vmm::unmap_pages(PageTable* pml4, void* virt, size_t count) {
    while (count--) {
        PageTableEntries offs = Vmm::virtual_to_entries(virt);

        PageTable* pml4_virt = reinterpret_cast<PageTable*>(reinterpret_cast<uint64_t>(pml4) + virtual_physical_base);
        PageTable* pdp_virt = get_or_null_ent(pml4_virt, offs.pml4);
        if (!pdp_virt)
            return false;
        PageTable* pd_virt = get_or_null_ent(pdp_virt, offs.pdp);
        if (!pd_virt)
            return false;
        PageTable* pt_virt = get_or_null_ent(pd_virt, offs.pd);
        if (!pt_virt)
            return false;
        pt_virt->ents[offs.pt] = 0;

        virt = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(virt) + 0x1000);
    }

    return true;
}

/// Updates a virtual address' permissions
bool Vmm::update_perms(PageTable* pml4, void* virt, size_t count, int perms) {
    while (count--) {
        PageTableEntries offs = Vmm::virtual_to_entries(virt);

        PageTable* pml4_virt = reinterpret_cast<PageTable*>(reinterpret_cast<uint64_t>(pml4) + virtual_physical_base);
        PageTable* pdp_virt = get_or_null_ent(pml4_virt, offs.pml4);
        if (!pdp_virt)
            return false;
        PageTable* pd_virt = get_or_null_ent(pdp_virt, offs.pdp);
        if (!pd_virt)
            return false;
        PageTable* pt_virt = get_or_null_ent(pd_virt, offs.pd);
        if (!pt_virt)
            return false;
        pt_virt->ents[offs.pt] = (pt_virt->ents[offs.pt] & address_mask) | perms | VirtualMemoryFlags::VMM_PRESENT;

        virt = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(virt) + 0x1000);
    }

    return true;
}

/// Maps a huge memory page
bool Vmm::map_huge_pages(PageTable* pml4, void* virt, void* phys, size_t count, int perms) {
    while (count--) {
        PageTableEntries offs = Vmm::virtual_to_entries(virt);

        PageTable* pml4_virt = reinterpret_cast<PageTable*>(reinterpret_cast<uint64_t>(pml4) + virtual_physical_base);
        PageTable* pdp_virt = get_or_alloc_ent(pml4_virt, offs.pml4, perms);
        PageTable* pd_virt = get_or_alloc_ent(pdp_virt, offs.pdp, perms);
        pd_virt->ents[offs.pd] = reinterpret_cast<uint64_t>(phys) | perms | VirtualMemoryFlags::VMM_PRESENT | VirtualMemoryFlags::VMM_LARGE;

        virt = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(virt) + 0x200000);
        phys = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(phys) + 0x200000);
    }

    return true;
}

/// Unmaps a huge memory page
bool Vmm::unmap_huge_pages(PageTable* pml4, void* virt, size_t count) {
    while (count--) {
        PageTableEntries offs = Vmm::virtual_to_entries(virt);

        PageTable* pml4_virt = reinterpret_cast<PageTable*>(reinterpret_cast<uint64_t>(pml4) + virtual_physical_base);
        PageTable* pdp_virt = get_or_null_ent(pml4_virt, offs.pml4);
        if (!pdp_virt)
            return false;
        PageTable* pd_virt = get_or_null_ent(pdp_virt, offs.pdp);
        pd_virt->ents[offs.pd] = 0;

        virt = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(virt) + 0x200000);
    }

    return true;
}

/// Updates a huge memory page's virtual address' permissions
bool Vmm::update_huge_perms(PageTable* pml4, void* virt, size_t count, int perms) {
    while (count--) {
        PageTableEntries offs = Vmm::virtual_to_entries(virt);

        PageTable* pml4_virt = reinterpret_cast<PageTable*>(reinterpret_cast<uint64_t>(pml4) + virtual_physical_base);
        PageTable* pdp_virt = get_or_null_ent(pml4_virt, offs.pml4);
        if (!pdp_virt)
            return false;
        PageTable* pd_virt = get_or_null_ent(pdp_virt, offs.pdp);
        pd_virt->ents[offs.pd] = (pd_virt->ents[offs.pd] & address_mask) | perms | VirtualMemoryFlags::VMM_PRESENT | VirtualMemoryFlags::VMM_LARGE;

        virt = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(virt) + 0x200000);
    }

    return true;
}

/// Gets physical address
uintptr_t Vmm::get_entry(PageTable* pml4, void* virt) {
    if ((uintptr_t)virt >= 0xFFFFFFFF80000000)
        return (uintptr_t)virt - 0xFFFFFFFF80000000;
    if ((uintptr_t)virt >= 0xFFFF800000000000)
        return (uintptr_t)virt - 0xFFFF800000000000;

    PageTableEntries offs = Vmm::virtual_to_entries(virt);

    PageTable* pml4_virt = reinterpret_cast<PageTable*>(reinterpret_cast<uint64_t>(pml4) + virtual_physical_base);
    PageTable* pdp_virt = get_or_null_ent(pml4_virt, offs.pml4);
    if (!pdp_virt)
        return 0;
    PageTable* pd_virt = get_or_null_ent(pdp_virt, offs.pdp);
    if (!pd_virt)
        return 0;
    PageTable* pt_virt = get_or_null_ent(pd_virt, offs.pd);
    if (!pt_virt)
        return 0;
    return pt_virt->ents[offs.pt];
}

/// Creates a new address space
PageTable* Vmm::new_address_space() {
    PageTable* new_pml4 = static_cast<PageTable*>(Pmm::alloc(1));
    memset(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(new_pml4) + virtual_physical_base), 0, 4096);

    map_huge_pages(new_pml4, reinterpret_cast<void*>(0xFFFFFFFF80000000), NULL, 64, 3);
    map_huge_pages(new_pml4, reinterpret_cast<void*>(0xFFFF800000000000), NULL, 512 * 4, 3);

    return new_pml4;
}

/// Gives the saved context's pointer
PageTable** Vmm::get_ctx_ptr() {
    return reinterpret_cast<PageTable**>(get_current_context());  // I don't know what I did but DON'T TOUCH IT!!
}

/// Saves the current context
void Vmm::save_context() {
    PageTable** ctx = get_ctx_ptr();
    *ctx = get_current_context();
}

/// Gives the saved context's pointer
PageTable* Vmm::get_saved_context() {
    PageTable** ctx = get_ctx_ptr();
    return *ctx;
}

/// Restores the context
void Vmm::restore_context() {
    PageTable** ctx = get_ctx_ptr();
    set_context(*ctx);
    *ctx = NULL;
}

/// Drops the context
void Vmm::drop_context() {
    PageTable** ctx = get_ctx_ptr();
    *ctx = NULL;
}

/// Sets the context
void Vmm::set_context(PageTable* ctx) {
    asm volatile("mov %%rax, %%cr3"
                 :
                 : "a"(ctx)
                 : "memory");
}

/// Gives the current context
PageTable* Vmm::get_current_context() {
    uintptr_t ctx = 0;
    asm volatile("mov %%cr3, %%rax"
                 : "=a"(ctx)
                 :
                 : "memory");
    return reinterpret_cast<PageTable*>(ctx);
}

/// Updates the mapping
void Vmm::update_mapping(void* ptr) {
    asm volatile("invlpg (%0)"
                 :
                 : "r"(ptr)
                 : "memory");
}

/// Copies the context
void Vmm::ctx_memcpy(PageTable* dst_ctx, void* dst_addr, PageTable* src_ctx, void* src_addr, size_t size) {
    uintptr_t src_virt = 0x780000000000;
    uintptr_t dst_virt = 0x700000000000;
    uintptr_t dst = reinterpret_cast<uintptr_t>(dst_addr) & (~0xFFF);
    uintptr_t src = reinterpret_cast<uintptr_t>(src_addr) & (~0xFFF);
    size_t map_size = size + Math::max(reinterpret_cast<uintptr_t>(dst_addr) & 0xFFF, reinterpret_cast<uintptr_t>(src_addr) & 0xFFF);

    for (size_t i = 0; i < map_size; i += 0x1000, src_virt += 0x1000, src += 0x1000, dst_virt += 0x1000, dst += 0x1000) {
        uintptr_t dst_phys = get_entry(dst_ctx, reinterpret_cast<void*>(dst)) & address_mask;
        uintptr_t src_phys = get_entry(src_ctx, reinterpret_cast<void*>(src)) & address_mask;

        map_pages(kernel_pml4, reinterpret_cast<void*>(dst_virt), reinterpret_cast<void*>(dst_phys), 1, VirtualMemoryFlags::VMM_WRITE);
        map_pages(kernel_pml4, reinterpret_cast<void*>(src_virt), reinterpret_cast<void*>(src_phys), 1, VirtualMemoryFlags::VMM_WRITE);
        update_mapping(reinterpret_cast<void*>(dst_virt));
        update_mapping(reinterpret_cast<void*>(src_virt));
    }

    memcpy(reinterpret_cast<void*>(0x700000000000 + (reinterpret_cast<uintptr_t>(dst_addr) & 0xFFF)), reinterpret_cast<void*>(0x780000000000 + (reinterpret_cast<uintptr_t>(src_addr) & 0xFFF)), size);

    src_virt = 0x780000000000;
    dst_virt = 0x700000000000;

    for (size_t i = 0; i < size; i += 0x1000, src_virt += 0x1000, dst_virt += 0x1000) {
        unmap_pages(kernel_pml4, reinterpret_cast<void*>(dst_virt), 1);
        unmap_pages(kernel_pml4, reinterpret_cast<void*>(src_virt), 1);
        update_mapping(reinterpret_cast<void*>(dst_virt));
        update_mapping(reinterpret_cast<void*>(src_virt));
    }
}

int Vmm::to_flags(int flags) {
    return ((flags & MemoryFlags::WRITE) ? VirtualMemoryFlags::VMM_WRITE : 0) | ((flags & MemoryFlags::USER) ? VirtualMemoryFlags::VMM_USER : 0) | ((flags & MemoryFlags::NO_CACHE) ? (VirtualMemoryFlags::VMM_NO_CACHE | VirtualMemoryFlags::VMM_WT) : 0);
}