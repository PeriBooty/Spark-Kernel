#include <stdint.h>
#include <hardware/cpu/cpu.hpp>
#include <hardware/devices/vbe.hpp>
#include <hardware/mm/mm.hpp>
#include <hardware/mm/pmm.hpp>
#include <hardware/mm/vmm.hpp>
#include <multiboot.hpp>
#include <sys/idt.hpp>
#include <sys/panic.hpp>

/// The kernel entry function, called from boot.asm
extern "C" void kernel_main(void *mb_info_ptr, uint32_t multiboot_magic) {
    if (multiboot_magic == 0x2BADB002 && mb_info_ptr) {
        MultibootInfo &mb_info = *(MultibootInfo *)((uint64_t)mb_info_ptr + virtual_physical_base);  // Multiboot info
        MultibootMemoryMap *memory_map = (MultibootMemoryMap *)(uint64_t)mb_info.mmap_addr;   // Memory map array
        pmm_init(memory_map, mb_info.mmap_length / sizeof(*memory_map));                      // Initialize bitmap physical memory management
        bool pat_supported = cpu_check_pat();                                                 // Check if the PAT MSR is supported
        vmm_init(pat_supported);                                                              // Initialize 4-level paging
        uint64_t virtual_fb_addr = mb_info.framebuffer_addr + virtual_physical_base;                 // Virtual address of VBE framebuffer
        // Map the VBE framebuffer virtual address without PAT so we can panic
        if (map_pages(vmm_get_current_context(), (void *)virtual_fb_addr, (void *)mb_info.framebuffer_addr, (mb_info.framebuffer_width * mb_info.framebuffer_pitch + page_size - 1) / page_size, VirtualMemoryFlags::VMM_WRITE | MemoryFlags::READ)) {
            GraphicsModeInfo mode_info = {
                .framebuffer = (uint32_t *)virtual_fb_addr,
                .pitch = mb_info.framebuffer_pitch,
                .width = mb_info.framebuffer_width,
                .height = mb_info.framebuffer_height,
            };                         // Create graphics mode info struct
            Display::init(mode_info);  // Initialize display
            if (!pat_supported)
                kernel_panic("PAT is not supported on this processor.");  // PAT is not supported, panic
            // Since PAT MSR is supported, we can update the permissions of the maps to use PAT
            vmm_update_perms(vmm_get_current_context(), (void *)virtual_fb_addr, (mb_info.framebuffer_width * mb_info.framebuffer_pitch + page_size - 1) / page_size, VirtualMemoryFlags::VMM_WRITE | MemoryFlags::READ | VirtualMemoryFlags::VMM_LARGE | VirtualMemoryFlags::VMM_WT);
            // Draw some text
            Display::write("OK THIS IS EPIC,", mb_info.framebuffer_width / 2 - 16 * 8, mb_info.framebuffer_height - 100 - 16, 0xFFFFFF);
            Display::write("I think...", mb_info.framebuffer_width / 2 - 10 * 8, mb_info.framebuffer_height - 100, 0xFFFFFF);
            idt_init();  // Initialize the Interrupt Descriptor Table
        } else
            return;  // Not enough memory (Out of bounds?)
    } else
        return;  // Bootloader not multiboot compliant or bootloader did not pass multiboot info
    while (1)
        asm volatile("hlt");  // Halt CPU so we don't hug its usage
}