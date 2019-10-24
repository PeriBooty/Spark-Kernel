#include <lib/lib.hpp>

void* memset(void* s, int c, size_t n) {
    unsigned char* p = (unsigned char*)s;
    while (n--)
        *p++ = (unsigned char)c;
    return s;
}

size_t strlen(const char* chr) {
    size_t size = 0;
    while (*chr++)
        size++;
    return size;
}

size_t strnlen(const char* chr) {
    return strlen(chr) + 1;
}

size_t strlen(const char16_t* chr) {
    size_t size = 0;
    while (*chr++)
        size++;
    return size;
}

size_t strnlen(const char16_t* chr) {
    return strlen(chr) + 1;
}

char* itoa(int value, char* result, int base) {
    if (base < 2 || base > 36) {
        *result = '\0';
        return result;
    }

    char *ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
    } while (value);
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}