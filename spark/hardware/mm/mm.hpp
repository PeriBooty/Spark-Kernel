#pragma once
#include <stddef.h>
#include <stdint.h>
constexpr inline size_t virtual_kernel_base = 0xFFFFFFFF80000000;
constexpr inline size_t virtual_physical_base = 0xFFFF800000000000;
constexpr inline size_t page_size = 0x1000;
constexpr inline size_t memory_base = 0x2000000;
constexpr inline size_t heap_base = 0x200000000;

/**
 * @brief The memory flags
 * 
 */
enum MemoryFlags {
    READ = 0x01,
    WRITE = 0x02,
    EXECUTE = 0x04,
    USER = 0x08,
    NO_CACHE = 0x10,
};

/**
 * @brief Allocates memory
 * 
 * @param blocks amount of memory
 * @return void*
 */
void* malloc(size_t blocks);

/**
 * @brief Safely allocates memory by zeroing it
 * 
 * @param blocks amount of memory
 * @return void*
 */
void* calloc(size_t blocks);

/**
 * @brief Frees memory
 * 
 * @param memory 
 * @return true 
 * @return false 
 */
bool free(void* memory);

/**
 * @brief Fills memory with something
 * 
 * @param s 
 * @param c 
 * @param n 
 * @return void* 
 */
void* memset(void* s, int c, size_t n);

/**
 * @brief Copies memory
 * 
 * @param dest 
 * @param src 
 * @param len 
 * @return void* 
 */
void* memcpy(void* dest, const void* src, size_t len);