#include <hardware/cpu/smp/smp.hpp>
#include <hardware/mm/mm.hpp>
#include <hardware/mm/vmm.hpp>
#include <lib/lib.hpp>
#include <sys/acpi/apic.hpp>
#include <sys/terminal.hpp>

bool trampoline_booted = false;

bool Spark::Cpu::Smp::wait_for_boot() {
    for (uint64_t timeout = 100; timeout > 0; timeout--) {
        if (trampoline_booted)
            return true;

        for (uint64_t i = 100000; i > 0; i--)
            asm("nop");
    }

    return false;
}

void Spark::Cpu::Smp::init() {
    uint64_t len = (uintptr_t)&_trampoline_end - (uintptr_t)&_trampoline_start;
    
    Vmm::map_pages(Vmm::get_current_context(), &_trampoline_start, &_trampoline_start, (len + page_size - 1) / page_size, Vmm::VirtualMemoryFlags::VMM_PRESENT | Vmm::VirtualMemoryFlags::VMM_WRITE);
    memcpy(&_trampoline_start, (void*)(0x400000 + virtual_physical_base), len);
}

void Spark::Cpu::Smp::boot_cpu(CpuEntry cpu) {
    if (cpu.bsp)
        return;

    void* trampoline_stack = malloc(4);
    char debug[255] = "";

    if (trampoline_stack == nullptr) {
        sprintf(debug, "[SMP] Failed to allocate stack for CPU with lapic ID %d", cpu.lapic_id);
        Terminal::write_line(debug, 0xFFFFFF);
        return;
    }

    Apic::LocalApic::send_ipi(cpu.lapic_id, Apic::LocalApic::IcrFlags::TM_LEVEL | Apic::LocalApic::IcrFlags::LEVELASSERT | Apic::LocalApic::IcrFlags::DM_INIT);
    Apic::LocalApic::send_ipi(cpu.lapic_id, Apic::LocalApic::IcrFlags::DM_SIPI | (uint32_t)((uintptr_t)&smp_entry / page_size));

    if (!wait_for_boot()) Apic::LocalApic::send_ipi(cpu.lapic_id, Apic::LocalApic::IcrFlags::DM_SIPI | (uint32_t)((uintptr_t)&smp_entry / page_size));

    if (wait_for_boot()) {
        sprintf(debug, "[SMP] Sucessfully booted CPU with lapic ID %d", cpu.lapic_id);
        Terminal::write_line(debug, 0xFFFFFF);
    } else {
        sprintf(debug, "[SMP] Failed to boot CPU with lapic ID %d", cpu.lapic_id);
        Terminal::write_line(debug, 0xFFFFFF);
    }

    trampoline_booted = false;
}

void Spark::Cpu::Smp::set_booted() {
    trampoline_booted = true;
}