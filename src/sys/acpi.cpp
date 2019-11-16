#include <hardware/mm/mm.hpp>
#include <hardware/port.hpp>
#include <lib/lib.hpp>
#include <sys/acpi.hpp>
#include <sys/panic.hpp>
#include <sys/terminal.hpp>

RSDPInfo ACPI::rsdp_info = {};

inline uint8_t ACPI::bios_calculate_checksum(void* ptr, size_t size) {
    uint8_t sum = 0;
    for (size_t i = 0; i < size; i++)
        sum += ((uint8_t*)ptr)[i];
    return sum;
}

inline RSDPInfo ACPI::bios_detect_rsdp(uint64_t base, size_t length) {
    uint64_t address = base + virtual_physical_base;
    RSDPInfo info{};

    for (size_t off = 0; off < length; off += 16) {
        RSDPDescriptor* rsdp = reinterpret_cast<RSDPDescriptor*>(address + off);

        if (strncmp(reinterpret_cast<const signed char*>(rsdp->signature), reinterpret_cast<const signed char*>("RSD PTR "), 8) || bios_calculate_checksum(rsdp, sizeof(RSDPDescriptor)))
            continue;

        info.rsdp_address = base + off + virtual_physical_base;

        if (!rsdp->revision) {
            info.version = 1;
            info.address = static_cast<uint64_t>(rsdp->rsdt_address) + virtual_physical_base;
            break;
        } else {
            RSDPDescriptor2* rsdp2 = reinterpret_cast<RSDPDescriptor2*>(rsdp);

            if (bios_calculate_checksum(rsdp2, sizeof(RSDPDescriptor)))
                continue;

            info.version = 2;
            info.address = static_cast<uint64_t>(rsdp2->xsdt_address) + virtual_physical_base;
            break;
        }
    }

    return info;
}

inline RSDPInfo ACPI::bios_detect_rsdp() {
    RSDPInfo info = bios_detect_rsdp(static_cast<uint64_t>(*reinterpret_cast<uint16_t*>(static_cast<uint64_t>(0x40E) + virtual_physical_base)) << 4, 0x400);

    if (!info.version)
        info = bios_detect_rsdp(0xE0000, 0x20000);
    if (!info.version)
        panic("ACPI not supported");
    return info;
}

inline ACPISDTHeader* ACPI::bios_rsdt_search(const char* signature) {
    if (!rsdp_info.version)
        return NULL;

    if (rsdp_info.version < 2) {
        RSDT* rsdt = reinterpret_cast<RSDT*>(rsdp_info.address);

        uint32_t entries = (rsdt->header.length - sizeof(rsdt->header)) / 4;

        for (uint32_t i = 0; i < entries; i++) {
            ACPISDTHeader* h = reinterpret_cast<ACPISDTHeader*>(static_cast<uint64_t>(rsdt->other_std[i]) + virtual_physical_base);
            if (!strncmp(reinterpret_cast<const signed char*>(h->signature), reinterpret_cast<const signed char*>(signature), strlen(signature)))
                return h;
        }
    } else {
        XSDT* xsdt = reinterpret_cast<XSDT*>(rsdp_info.address);

        uint32_t entries = (xsdt->header.length - sizeof(xsdt->header)) / 4;

        for (uint32_t i = 0; i < entries; i++) {
            ACPISDTHeader* h = reinterpret_cast<ACPISDTHeader*>(static_cast<uint64_t>(xsdt->other_std[i]) + virtual_physical_base);
            if (!strncmp(reinterpret_cast<const signed char*>(h->signature), reinterpret_cast<const signed char*>(signature), strlen(signature)))
                return h;
        }
    }

    return NULL;
}

void ACPI::init() {
    rsdp_info = bios_detect_rsdp();

    printf("[ACPI] Detected ACPI v%d, OEM: %s", rsdp_info.version, reinterpret_cast<RSDPDescriptor*>(rsdp_info.rsdp_address)->oem_id);
    /*FADT* fadt = reinterpret_cast<FADT*>(bios_rsdt_search("FACP"));
    Port::outb(fadt->smi_command_port, fadt->acpi_enable);
    Terminal::write_line("[ACPI] Waiting for PM1a control block...", 0xFFFFFF);
    while ((Port::inw(fadt->pm1a_control_block) & 1) == 0)
        ;*/
    Terminal::write_line("[ACPI] Initialized ACPI successfully", 0xFFFFFF);
}