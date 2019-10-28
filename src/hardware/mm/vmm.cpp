#include <stddef.h>
#include <stdint.h>
#include <hardware/mm/mm.hpp>

[[gnu::always_inline]] static inline uint8_t get_pml_index(int pml, uint64_t address) {
    return (pml >= 4 ? address >> 39 : address >> 12 + (pml - 1) * 9) & 0x1FF;
}

int map_address(uint64_t *pml4, uint16_t flags, uint64_t physical_address, size_t address) {
    uint64_t *pml3, *pml2, *pml1;
    uint8_t pml4_index = get_pml_index(4, address), pml3_index = get_pml_index(3, address),
            pml2_index = get_pml_index(2, address), pml1_index = get_pml_index(1, address);
    if (pml4[pml4_index] & 0x1)
        pml3 = (uint64_t *)((pml4[pml4_index] & 0xFFFFFFFFFFFFF000) + PHYSICAL_MEM_MAPPING);
    else {
        pml3 = (uint64_t *)((size_t)calloc(1) + PHYSICAL_MEM_MAPPING);
        if ((size_t)pml3 == PHYSICAL_MEM_MAPPING) goto fail1;
        pml4[pml4_index] = *pml3 | 0b111;
    }
    if (pml3[pml3_index] & 0x1)
        pml2 = (uint64_t *)((pml3[pml3_index] & 0xFFFFFFFFFFFFF000) + PHYSICAL_MEM_MAPPING);
    else {
        pml2 = (uint64_t *)((size_t)calloc(1) + PHYSICAL_MEM_MAPPING);
        if ((size_t)pml2 == PHYSICAL_MEM_MAPPING) goto fail2;
        pml3[pml3_index] = *pml2 | 0b111;
    }
    if (pml2[pml2_index] & 0x1)
        pml1 = (uint64_t *)((pml2[pml2_index] & 0xFFFFFFFFFFFFF000) + PHYSICAL_MEM_MAPPING);
    else {
        pml1 = (uint64_t *)((size_t)calloc(1) + PHYSICAL_MEM_MAPPING);
        if ((size_t)pml1 == PHYSICAL_MEM_MAPPING) goto fail3;
        pml2[pml2_index] = *pml1 | 0b111;
    }

    pml1[pml1_index] = physical_address | flags;
    asm volatile("invlpg (%0)" ::"r" (address) : "memory");
    return 0;

fail3:
    for (size_t i = 0;; i++) {
        if (i == PAGE_TABLE_ENTRIES) {
            free((void *)(pml1 - PHYSICAL_MEM_MAPPING), 1);
            break;
        }
        if (pml1[i] & 0x1)
            goto fail1;
    }

fail2:
    for (size_t i = 0;; i++) {
        if (i == PAGE_TABLE_ENTRIES) {
            free((void *)(pml2 - PHYSICAL_MEM_MAPPING), 1);
            break;
        }
        if (pml2[i] & 0x1)
            goto fail1;
    }

fail1:
    return -1;
}