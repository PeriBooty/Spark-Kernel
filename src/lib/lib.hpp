#pragma once
#include <stddef.h>
#include <stdint.h>

/// Get length of string
size_t strlen(const char*);

/// Get length of string
size_t strlen(const char16_t*);

/// Get length of string until max length
size_t strnlen(const char* chr, size_t max_len);

/// Get length of string until max length
size_t strnlen(const char16_t* chr, size_t max_len);

/// Converts an integer to a string
char* itoa(int value, char* result, int base);

/// Copy
template <class InputIt, class OutputIt>
OutputIt copy(InputIt first, InputIt last, OutputIt d_first) {
    while (first != last) {
        *d_first++ = *first++;
    }
    return d_first;
}

/// Copy with condition
template <class InputIt, class OutputIt, class UnaryPredicate>
OutputIt copy_if(InputIt first, InputIt last, OutputIt d_first, UnaryPredicate pred) {
    while (first != last) {
        if (pred(*first)) *d_first++ = *first;
        first++;
    }
    return d_first;
}