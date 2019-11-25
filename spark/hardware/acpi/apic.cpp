#include <hardware/mm/mm.hpp>
#include <hardware/mm/vmm.hpp>
#include <hardware/msr.hpp>
#include <hardware/acpi/apic.hpp>

uint64_t lapic_base;

uint32_t Spark::Apic::LocalApic::read(uint32_t reg) {
    volatile uint32_t* value = (uint32_t*)(lapic_base + reg);
    return *value;
}

void Spark::Apic::LocalApic::write(uint32_t reg, uint32_t data) {
    volatile uint32_t* value = (uint32_t*)(lapic_base + reg);
    *value = data;
}

void Spark::Apic::LocalApic::init() {
    uint64_t apic_base = Msr::read(Msr::apic_base);
    lapic_base = (apic_base & 0xFFFFFFFFFFFFF000) + virtual_kernel_base;
    apic_base |= 1 << 11;
    Vmm::map_pages(Vmm::get_current_context(), (void*)lapic_base, (void*)(apic_base & 0xFFFFFFFFFFFFF000), 1, Vmm::VirtualMemoryFlags::VMM_WRITE);
    Msr::write(Msr::apic_base, apic_base);
}

void Spark::Apic::LocalApic::send_ipi(uint32_t target, uint32_t flags) {
    write(icr_high, target << 24);
    write(icr_low, flags);
    while(read(icr_low) & IcrFlags::PENDING);
}