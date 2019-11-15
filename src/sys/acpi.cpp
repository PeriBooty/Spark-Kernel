#include <hardware/mm/mm.hpp>
#include <hardware/port.hpp>
#include <lib/lib.hpp>
#include <sys/acpi.hpp>
#include <sys/panic.hpp>
#include <sys/terminal.hpp>

static Rsdt *rsdt;

void init_acpi() {
    RsdpDescriptor *rsdp_descriptor = find_rsdp();

    printf("[ACPI] Detected ACPI v%d, OEM: %s", rsdp_descriptor->revision + 1, rsdp_descriptor->oem_id);
    if (rsdp_descriptor->revision > 0) {
        //RsdpDescriptor2 &rsdp_descriptor2 = *(RsdpDescriptor2 *)rsd_ptr;
    } else {
        rsdt = reinterpret_cast<Rsdt *>(static_cast<uint64_t>(rsdp_descriptor->rsdt_addr) + virtual_physical_base);
        Fadt *fadt = reinterpret_cast<Fadt *>(rsdt_search("FACP"));
        Port::outb(fadt->smi_command_port, fadt->acpi_enable);
        Terminal::write_line("[ACPI] Waiting for PM1a control block...", 0xFFFFFF);
        while ((Port::inw(fadt->pm1a_control_block) & 1) == 0)
            ;
    }
    Terminal::write_line("[ACPI] Initialized ACPI successfully", 0xFFFFFF);
}

inline RsdpDescriptor *find_rsdp() {
    uint64_t rsd_ptr = 0;

    for (uint64_t i = 0x80000 + virtual_physical_base; i < 0xa0000 + virtual_physical_base && !rsd_ptr; i += 8)
        rsd_ptr = !strncmp(reinterpret_cast<const char *>(i), "RSD PTR ", 8) ? i : 0;

    if (!rsd_ptr)
        for (uint64_t i = 0xf0000 + virtual_physical_base; i < 0x100000 + virtual_physical_base && !rsd_ptr; i += 8)
            rsd_ptr = !strncmp(reinterpret_cast<const char *>(i), "RSD PTR ", 8) ? i : 0;

    if (!rsd_ptr)
        kernel_panic("ACPI not supported");
        
    return reinterpret_cast<RsdpDescriptor *>(rsd_ptr);
}

inline AcpiSdtHeader *rsdt_search(const char *signature) {
    if (!rsdt)
        return NULL;

    uint32_t entries = (rsdt->h.len - sizeof(rsdt->h)) / 4;

    for (uint32_t i = 0; i < entries; i++) {
        AcpiSdtHeader *h = reinterpret_cast<AcpiSdtHeader *>(static_cast<uint64_t>(rsdt->other_std_ptr[i]) + virtual_physical_base);
        if (!strncmp(h->signature, signature, strlen(signature)))
            return h;
    }

    return NULL;  // No FACP found
}