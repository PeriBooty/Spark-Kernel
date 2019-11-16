#include <stdint.h>
#include <hardware/cpu/cpu.hpp>
#include <hardware/devices/vbe.hpp>
#include <hardware/mm/mm.hpp>
#include <hardware/mm/pmm.hpp>
#include <hardware/mm/vmm.hpp>
#include <multiboot.hpp>
#include <sys/acpi.hpp>
#include <sys/idt.hpp>
#include <sys/panic.hpp>
#include <sys/terminal.hpp>

/// The kernel entry function, called from boot.asm
extern "C" void kernel_main(void* mb_info_ptr, uint32_t multiboot_magic) {
    if (multiboot_magic == 0x2BADB002 && mb_info_ptr) {
        MultibootInfo& mb_info = *reinterpret_cast<MultibootInfo*>(reinterpret_cast<uint64_t>(mb_info_ptr) + virtual_physical_base);  // Multiboot info
        MultibootMemoryMap* memory_map = reinterpret_cast<MultibootMemoryMap*>(static_cast<uint64_t>(mb_info.mmap_addr));             // Memory map array
        PMM::init(memory_map, mb_info.mmap_length / sizeof(*memory_map));                                                              // Initialize bitmap physical memory management
        bool pat_supported = CPU::check_pat();                                                                                        // Check if the PAT MSR is supported
        VMM::init(pat_supported);                                                                                                      // Initialize 4-level paging
        void* virtual_fb = reinterpret_cast<void*>(mb_info.framebuffer_addr + virtual_physical_base);                                 // Virtual address of VBE framebuffer
        // Map the VBE framebuffer virtual address without PAT so we can panic
        if (VMM::map_pages(VMM::get_current_context(), virtual_fb, reinterpret_cast<void*>(mb_info.framebuffer_addr), (mb_info.framebuffer_width * mb_info.framebuffer_pitch + page_size - 1) / page_size, VirtualMemoryFlags::VMM_WRITE | MemoryFlags::READ)) {
            GraphicsModeInfo mode_info = {
                .framebuffer = reinterpret_cast<uint32_t*>(virtual_fb),
                .pitch = mb_info.framebuffer_pitch,
                .width = mb_info.framebuffer_width,
                .height = mb_info.framebuffer_height,
            };                         // Create graphics mode info struct
            Display::init(mode_info);  // Initialize display
            if (pat_supported)
                VMM::update_perms(VMM::get_current_context(), virtual_fb, (mb_info.framebuffer_width * mb_info.framebuffer_pitch + page_size - 1) / page_size, VirtualMemoryFlags::VMM_WRITE | MemoryFlags::READ | VirtualMemoryFlags::VMM_LARGE | VirtualMemoryFlags::VMM_WT);  // Since PAT MSR is supported, we can update the permissions of the maps to use PAT
            ACPI::init();                                                                                                                                                                                                                                                      // Initialize ACPI
            IDT::init();                                                                                                                                                                                                                                                       // Initialize the Interrupt Descriptor Table
            Terminal::write_line(
                "             zeeeeee-\r\n\
            z$$$$$$\"\r\n\
           d$$$$$$\"\r\n\
          d$$$$$P\r\n\
         d$$$$$P\r\n\
        $$$$$$\"\r\n\
      .$$$$$$\"\r\n\
     .$$$$$$\"\r\n\
    4$$$$$$$$$$$$$\"\r\n\
   z$$$$$$$$$$$$$\"\r\n\
   \"\"\"\"\"\"\"3$$$$$\"\r\n\
         z$$$$P\r\n\
        d$$$$\"              Welcome to SparkOS version 0.0.1::1911!\r\n\
      .$$$$$\"\r\n\
     z$$$$$\"\r\n\
    z$$$$P\r\n\
   d$$$$$$$$$$\"\r\n\
  *******$$$\"\r\n\
       .$$$\"\r\n\
      .$$\"\r\n\
     4$P\"\r\n\
    z$\"\r\n\
   zP\r\n\
  z\"\r\n\
 /\r\n\
^",
                0x0031ad);  // Welcome user to OS
        } else
            return;  // Not enough memory (Out of bounds?)
    } else
        return;  // Bootloader not multiboot compliant or bootloader did not pass multiboot info
    while (1)
        asm volatile("hlt");  // Halt CPU so we don't hug its usage
}