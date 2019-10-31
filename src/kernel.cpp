#include <stdint.h>
#include <hardware/devices/vbe.hpp>
#include <hardware/mm/mm.hpp>
#include <hardware/mm/pmm.hpp>
#include <hardware/mm/vmm.hpp>
#include <multiboot.hpp>

extern "C" void kernel_main(void *mb_info_ptr) {
    MultibootInfo &mb_info = *(MultibootInfo *)((uint64_t)mb_info_ptr + VIRT_PHYS_BASE);
    MultibootMemoryMap *memory_map = (MultibootMemoryMap *)(uint64_t)mb_info.mmap_addr;
    pmm_init(memory_map, mb_info.mmap_length / sizeof(*memory_map));
    vmm_init();
    uint64_t virtual_fb_addr = mb_info.framebuffer_addr + VIRT_PHYS_BASE;
    if (map_pages(get_current_context(), (void *)virtual_fb_addr, (void *)mb_info.framebuffer_addr, (mb_info.framebuffer_width * mb_info.framebuffer_pitch + PAGE_SIZE - 1) / PAGE_SIZE, VirtualMemoryFlags::VMM_PRESENT | VirtualMemoryFlags::VMM_WRITE)) {
        VideoModeInfo mode_info = {
            .framebuffer = (uint64_t *)virtual_fb_addr,
            .pitch = mb_info.framebuffer_pitch,
            .width = mb_info.framebuffer_width,
            .height = mb_info.framebuffer_height,
        };
        Display::init(mode_info);
        Display::write("OK THIS IS EPIC", mb_info.framebuffer_width - 50, mb_info.framebuffer_height - 50, 0xFF);
    } else {
        // Out of bounds
    }
    while (1)
        asm volatile("hlt");
}