#include <stdint.h>
#include <hardware/cpu/cpu.hpp>
#include <hardware/cpu/smp/smp.hpp>
#include <hardware/devices/vbe.hpp>
#include <hardware/mm/mm.hpp>
#include <hardware/mm/pmm.hpp>
#include <hardware/mm/vmm.hpp>
#include <lib/lib.hpp>
#include <multiboot.hpp>
#include <hardware/acpi/acpi.hpp>
#include <hardware/idt.hpp>
#include <hardware/terminal.hpp>

namespace Spark {
    /**
     * @brief The kernel entry method
     * 
     */
    extern "C" void kernel_main(void* mb_info_ptr, uint32_t multiboot_magic) {
        if (multiboot_magic == 0x2BADB002 && mb_info_ptr) {
            Multiboot::Info* mb_info = (Multiboot::Info*)((uint64_t)mb_info_ptr + virtual_kernel_base);  // Multiboot info
            Multiboot::MemoryMap* memory_map = (Multiboot::MemoryMap*)(uint64_t)mb_info->mmap_addr;      // Memory map array
            Pmm::init(memory_map, mb_info->mmap_length / sizeof(*memory_map));                           // Initialize bitmap physical memory management
            Vmm::init();                                                                                 // Initialize 4-level paging
            void* virtual_fb = (void*)(mb_info->framebuffer_addr + virtual_kernel_base);                 // Virtual address of VBE framebuffer

            // Map the VBE framebuffer
            if (Vmm::map_pages(Vmm::get_current_context(), virtual_fb, (void*)mb_info->framebuffer_addr, (mb_info->framebuffer_width * mb_info->framebuffer_pitch + page_size - 1) / page_size, Vmm::VirtualMemoryFlags::VMM_PRESENT | Vmm::VirtualMemoryFlags::VMM_WRITE)) {
                Graphics::ModeInfo mode_info = {
                    .backbuffer = (uint32_t*)malloc(mb_info->framebuffer_width * mb_info->framebuffer_pitch),
                    .framebuffer = (uint32_t*)virtual_fb,
                    .pitch = mb_info->framebuffer_pitch,
                    .width = mb_info->framebuffer_width,
                    .height = mb_info->framebuffer_height,
                    .bpp = mb_info->framebuffer_bpp,
                };  // Create graphics mode info struct

                Graphics::init(mode_info);  // Initialize display
                Idt::init();                // Initialize the Interrupt Descriptor Table
                Acpi::init();               // Initialize ACPI
            } else
                return;  // Not enough memory (Out of bounds?)
        } else
            return;  // Bootloader not multiboot compliant

        while (1)
            asm volatile("hlt");
    }

    extern "C" void smp_kernel_main() {
        Idt::init();
        Cpu::Smp::set_booted();

        while (1)
            asm volatile("hlt");
    }
}  // namespace Spark