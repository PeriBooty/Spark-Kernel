#include <stdint.h>
#include <hardware/devices/vbe.hpp>
#include <hardware/mm/mm.hpp>
#include <hardware/mm/pmm.hpp>
#include <hardware/mm/vmm.hpp>
#include <multiboot.hpp>
#include <sys/idt.hpp>
#include <sys/panic.hpp>

extern "C" void kernel_main(void *mb_info_ptr, uint32_t multiboot_magic) {
    if (multiboot_magic == 0x2BADB002 && mb_info_ptr) {
        MultibootInfo &mb_info = *(MultibootInfo *)((uint64_t)mb_info_ptr + VIRT_PHYS_BASE);
        MultibootMemoryMap *memory_map = (MultibootMemoryMap *)(uint64_t)mb_info.mmap_addr;
        pmm_init(memory_map, mb_info.mmap_length / sizeof(*memory_map));
        vmm_init();
        uint64_t virtual_fb_addr = mb_info.framebuffer_addr + VIRT_PHYS_BASE;
        if (map_pages(get_current_context(), (void *)virtual_fb_addr, (void *)mb_info.framebuffer_addr, (mb_info.framebuffer_width * mb_info.framebuffer_pitch + PAGE_SIZE - 1) / PAGE_SIZE, VirtualMemoryFlags::VMM_PRESENT | VirtualMemoryFlags::VMM_WRITE | MemoryFlags::READ)) {
            VideoModeInfo mode_info = {
                .framebuffer = (uint32_t *)virtual_fb_addr,
                .pitch = mb_info.framebuffer_pitch,
                .width = mb_info.framebuffer_width,
                .height = mb_info.framebuffer_height,
            };
            Display::init(mode_info);
            Display::clear(0x000000);
            Display::write("OK THIS IS EPIC,", mb_info.framebuffer_width / 2 - 16 * 8, mb_info.framebuffer_height - 100 - 16, 0xFFFFFF);
            Display::write("I think...", mb_info.framebuffer_width / 2 - 10 * 8, mb_info.framebuffer_height - 100, 0xFFFFFF);
            idt_init();
        } else
            return;  // Not enough memory (Out of bounds?)
    } else
        return;  // Bootloader not multiboot compliant or bootloader did not pass multiboot info
    while (1)
        asm volatile("hlt");
}