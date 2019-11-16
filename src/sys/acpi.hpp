#pragma once
#include <stdint.h>

struct [[gnu::packed]] RSDPDescriptor {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
};

struct [[gnu::packed]] RSDPDescriptor2 {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
};

struct RSDPInfo {
    int version;
    uint64_t rsdp_address;
    uint64_t address;
};

struct ACPISDTHeader {
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

struct RSDT {
    ACPISDTHeader header;
    uint32_t other_std[];
};

struct XSDT {
    ACPISDTHeader header;
    uint64_t other_std[];
};

struct GenericAddress {
    uint8_t address_space;
    uint8_t bit_width;
    uint8_t bit_offset;
    uint8_t access_size;
    uint64_t base;
};

struct FADT {
    ACPISDTHeader header;
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

class ACPI {
private:
    static RSDPInfo rsdp_info;
    static inline ACPISDTHeader* bios_rsdt_search(const char* signature);
    static inline RSDPInfo bios_detect_rsdp(uint64_t base, size_t length);
    static inline RSDPInfo bios_detect_rsdp();
    static inline uint8_t bios_calculate_checksum(void* ptr, size_t size);

public:
    static void init();
};