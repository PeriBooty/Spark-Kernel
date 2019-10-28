#pragma once
#include <stddef.h>
#include <stdint.h>
#include <multiboot.hpp>
#define KERNEL_VMA ((size_t)0xFFFFFFFF80000000)
#define PHYSICAL_MEM_MAPPING ((size_t)0xFFFF800000000000)
#define MEMORY_BASE ((size_t)0x1000000)
#define PAGE_SIZE ((size_t)4096)
#define BITMAP_BASE (MEMORY_BASE / PAGE_SIZE)
#define BMREALLOC_STEP 1
#define PAGE_TABLE_ENTRIES 512

extern void *(*malloc)(size_t pg_count);
extern void *calloc(size_t pg_count);
extern void free(void *ptr, size_t pg_count);
void change_alloc_method();

void init_pmm(MultibootMemoryMap *mmap);
int map_address(uint64_t *pml4, uint16_t flags, uint64_t physical_address, size_t address);