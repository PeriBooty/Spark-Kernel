#pragma once
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Formats a string
 * 
 * @param text 
 * @param format 
 * @param ... 
 * @return int 
 */
int sprintf(char* text, const char* format, ...);

/**
 * @brief Get if a string exists and the location of the string
 * 
 * @param char 
 * @param char 
 * @param n 
 * @return int 
 */
int strncmp(const char* str1, const char* str2, size_t len);

/**
 * @brief Get length of string
 * 
 * @return size_t 
 */
size_t strlen(const char*);

/**
 * @brief Get length of string until max length
 * 
 * @param chr 
 * @param max_len 
 * @return size_t 
 */
size_t strnlen(const char* chr, size_t max_len);

/**
 * @brief Converts an integer to a string
 * 
 * @param value 
 * @param result 
 * @param base 
 * @return char* 
 */
char* itoa(int value, char* result, int base);

/**
 * @brief Converts hex to a string
 * 
 * @param n 
 * @param str 
 * @param caps
 */
void htoa(uint64_t n, char* str, int caps);

/**
 * @brief Copy
 * 
 * @tparam InputIt 
 * @tparam OutputIt 
 * @param first 
 * @param last 
 * @param d_first 
 * @return OutputIt 
 */
template <class InputIt, class OutputIt>
OutputIt copy(InputIt first, InputIt last, OutputIt d_first) {
    for (; first != last; first++)
        *d_first++ = *first++;
    return d_first;
}

/**
 * @brief Copy with condition
 * 
 * @tparam InputIt 
 * @tparam OutputIt 
 * @tparam UnaryPredicate 
 * @param first 
 * @param last 
 * @param d_first 
 * @param pred 
 * @return OutputIt 
 */
template <class InputIt, class OutputIt, class UnaryPredicate>
OutputIt copy_if(InputIt first, InputIt last, OutputIt d_first, UnaryPredicate pred) {
    for (; first != last; first++)
        if (pred(*first))
            *d_first++ = *first;
    return d_first;
}