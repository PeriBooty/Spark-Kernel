#pragma once
#include <stdint.h>

namespace Spark::Acpi {
    struct [[gnu::packed]] RsdpDescriptor {
        char signature[8];
        uint8_t checksum;
        char oem_id[6];
        uint8_t revision;
        uint32_t rsdt_address;
    };

    struct [[gnu::packed]] RsdpDescriptor2 {
        RsdpDescriptor rsdp;
        uint32_t length;
        uint64_t xsdt_address;
        uint8_t extended_checksum;
        uint8_t reserved[3];
    };

    struct [[gnu::packed]] RsdpInfo {
        int version;
        uint64_t rsdp_address;
        uint64_t address;
        char oem_id[7];
    };

    struct [[gnu::packed]] SdtHeader {
        char signature[4];
        uint32_t length;
        uint8_t revision;
        uint8_t checksum;
        char oem_id[6];
        char oem_table_id[8];
        uint32_t oem_revision;
        uint32_t creator_id;
        uint32_t creator_revision;
    };

    enum InterruptControllerType {
        LAPIC = 0,
        IO_APIC = 1,
        INTERRUPT_SOURCE_OVERRIDE = 2,
        NMI_SOURCE = 3,
        LAPIC_NMI = 4,
        LAPIC_ADDRESS_OVERRIDE = 5,
        IO_SAPIC = 6,
        LOCAL_SAPIC = 7,
        PLATFORM_INTERRUPT_SOURCES = 8,
        PROCESSOR_LOCAL_X2APIC = 9,
        LOCAL_X2APIC_NMI = 10,
        GICC = 11,
        GICD = 12,
        GIC_MSI_FRAME = 13,
        GICR = 14,
        ITS = 15,
    };

    struct [[gnu::packed]] InterruptController {
        uint8_t type;
        uint8_t length;
    };

    struct [[gnu::packed]] LocalApic {
        InterruptController ic;
        uint8_t uid;
        uint8_t id;
        uint32_t flags;
    };

    struct [[gnu::packed]] MadtHeader {
        SdtHeader header;
        uint32_t apic_address;
        uint32_t flags;
        InterruptController interrupt_controllers[];
    };

    struct [[gnu::packed]] RsdtHeader {
        SdtHeader header;
        uint32_t other_std[];
    };

    struct [[gnu::packed]] XsdtHeader {
        SdtHeader header;
        uint64_t other_std[];
    };

    struct [[gnu::packed]] GenericAddress {
        uint8_t address_space;
        uint8_t bit_width;
        uint8_t bit_offset;
        uint8_t access_size;
        uint64_t base;
    };

    struct [[gnu::packed]] McfgEntry {
        uint64_t ecm_base;
        uint16_t segment;
        uint8_t start_bus_number;
        uint8_t end_bus_number;
        uint32_t reserved;
    };

    struct [[gnu::packed]] McfgHeader {
        SdtHeader header;
        uint64_t reserved;
        McfgEntry entries[];
    };

    struct [[gnu::packed]] FadtHeader {
        SdtHeader header;
        uint32_t firmware_control;
        uint32_t dsdt;
        uint8_t reserved;
        uint8_t power_management_profile;
        uint16_t sci_interrupt;
        uint32_t smi_command_port;
        uint8_t acpi_enable;
        uint8_t acpi_disable;
        uint8_t s4_bios_req;
        uint8_t pstate__control;
        uint32_t pm1a_event_block;
        uint32_t pm1b_event_block;
        uint32_t pm1a_control_block;
        uint32_t pm1b_control_block;
        uint32_t pm2_control_block;
        uint32_t pm_timer_block;
        uint32_t gpe0_block;
        uint32_t gpe1_block;
        uint8_t pm1_event_length;
        uint8_t pm1_control_length;
        uint8_t pm2_control_length;
        uint8_t pm_timer_length;
        uint8_t gpe0_length;
        uint8_t gpe1_length;
        uint8_t gpe1_base;
        uint8_t cstate_control;
        uint16_t worst_c2_latency;
        uint16_t worst_c3_latency;
        uint16_t flush_size;
        uint16_t flush_stride;
        uint8_t duty_offset;
        uint8_t duty_width;
        // CMOS registers
        uint8_t day_alarm;
        uint8_t month_alarm;
        uint8_t century;
        // ACPI 2.0+
        uint16_t boot_architecture_flags;
        uint8_t reserved2;
        uint32_t flags;
        GenericAddress reset_reg;
        uint8_t reset_value;
        uint8_t reserved3[3];
        uint64_t x_firmware_control;
        uint64_t x_dsdt;
        GenericAddress x_pm1a_event_block;
        GenericAddress x_pm1b_event_block;
        GenericAddress x_pm1a_control_block;
        GenericAddress x_pm1b_control_block;
        GenericAddress x_pm2_control_block;
        GenericAddress x_pm_timer_block;
        GenericAddress x_gpe0_block;
        GenericAddress x_gpe1_block;
    };

    /**
     * @brief Gets a table from a signature inside the RSDT/XSDT
     * 
     * @param signature 
     * @return Spark::Acpi::SdtHeader* 
     */
    Spark::Acpi::SdtHeader* get_table(const char* signature);

    /**
     * @brief Initializes ACPI
     * 
     */
    void init();
};  // namespace Spark::Acpi