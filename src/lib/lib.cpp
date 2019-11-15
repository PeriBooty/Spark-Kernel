#include <lib/ctype.hpp>
#include <lib/lib.hpp>
#include <sys/terminal.hpp>

int printf(const char *format, ...) {
    va_list parameters;
    va_start(parameters, format);

    uint64_t written = 0;

    while (*format != '\0') {
        size_t maxrem = 2147483647 - written;

        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%') format++;

            size_t amount = 0;
            while (format[amount] && format[amount] != '%')
                Terminal::write(format[amount++], 0xFFFFFF);

            if (maxrem < amount) {
                va_end(parameters);
                return -1;
            }

            format += amount;
            written += amount;
            continue;
        }

        const char *format_begun_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char)va_arg(parameters, int);
            if (!maxrem) {
                va_end(parameters);
                return -1;
            }

            Terminal::write(&c, 0xFFFFFF);
            va_end(parameters);

            written++;
        } else if (*format == 's') {
            format++;
            const char *str = va_arg(parameters, const char *);
            size_t len = strlen(str);

            if (maxrem < len) {
                va_end(parameters);
                return -1;
            }
            Terminal::write(str, 0xFFFFFF);

            written += len;
        } else if (*format == 'i' || *format == 'd') {
            format++;
            int item = va_arg(parameters, int);

            char str[32] = "";

            itoa(item, str, 10);
            size_t len = strlen(str);

            if (maxrem < len) {
                va_end(parameters);
                return -1;
            }
            Terminal::write(str, 0xFFFFFF);

            written += len;
        } else if (*format == 'x') {
            format++;
            uint64_t item = va_arg(parameters, uint64_t);

            char str[32] = "";

            htoa(item, str);
            size_t len = strlen(str);

            if (maxrem < len) {
                va_end(parameters);
                return -1;
            }
            Terminal::write(str, 0xFFFFFF);

            written += len;
        } else {
            format = format_begun_at;
            size_t len = strlen(format);

            if (maxrem < len) {
                va_end(parameters);
                return -1;
            }
            Terminal::write(format, 0xFFFFFF);

            written += len;
            format += len;
        }
    }
    Terminal::write("\r\n", 0xFFFFFF);
    va_end(parameters);
    return written;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        ++s1;
        ++s2;
        --n;
    }
    return !n ? 0 : (*(uint8_t *)s1 - *(uint8_t *)s2);
}

/// Get length of string
size_t strlen(const char *chr) {
    size_t size = 0;
    while (*chr++)
        size++;
    return size;
}

/// Get length of string
size_t strlen(const char16_t *chr) {
    size_t size = 0;
    while (*chr++)
        size++;
    return size;
}

/// Get length of string until max length
size_t strnlen(const char *chr, size_t max_len) {
    size_t size = 0;
    for (; (size < max_len) && chr[size]; ++size)
        ;
    return size;
}

/// Get length of string until max length
size_t strnlen(const char16_t *chr, size_t max_len) {
    size_t size = 0;
    for (; (size < max_len) && chr[size]; ++size)
        ;
    return size;
}

/// Converts an integer to a string
char *itoa(int value, char *result, int base) {
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

void htoa(int64_t n, char *str) {
    *str++ = '0';
    *str++ = 'x';

    int8_t zeros = 0;
    int64_t tmp;
    for (int i = 60; i > 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0) continue;

        zeros -= 1;

        if (tmp >= 0xA)
            *str++ = (tmp - 0xA + 'a');
        else
            *str++ = (tmp + '0');
    }

    tmp = n & 0xF;

    if (tmp >= 0xA)
        *str++ = (tmp - 0xA + 'a');
    else
        *str++ = (tmp + '0');
}