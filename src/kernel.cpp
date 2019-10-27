#include <stdint.h>
#include <hardware/devices/display.hpp>
#include <multiboot.hpp>
#include <ubsan.hpp>

extern "C" void kernel_main(void *mb_info_ptr) {
    multiboot_info &mb_info = *(multiboot_info *)mb_info_ptr;
    Display::init(mb_info.framebuffer_addr, mb_info.framebuffer_width, mb_info.framebuffer_height);
    Display::write_line("OK THIS IS EPIC");
    while (1)
        asm volatile("hlt");
}

extern "C" void __ubsan_handle_add_overflow(overflow_info *info, uintptr_t lhs, uintptr_t rhs) {
}

extern "C" void __ubsan_handle_sub_overflow(overflow_info *info, uintptr_t lhs, uintptr_t rhs) {
}

extern "C" void __ubsan_handle_pointer_overflow(overflow_info *info, uintptr_t lhs, uintptr_t rhs) {
}

extern "C" void __ubsan_handle_mul_overflow(overflow_info *info, uintptr_t lhs, uintptr_t rhs) {
}

extern "C" void __ubsan_handle_divrem_overflow(overflow_info *info, uintptr_t lhs, uintptr_t rhs) {
}

extern "C" void __ubsan_handle_negate_overflow(overflow_info *info, uintptr_t old) {
}

extern "C" void __ubsan_handle_shift_out_of_bounds(shift_out_of_bounds_info *info, uintptr_t lhs, uintptr_t rhs) {
}

extern "C" void __ubsan_handle_out_of_bounds(out_of_bounds_info *info, uintptr_t index) {
}

extern "C" void __ubsan_handle_nonnull_return(non_null_return_info *info, source_location *location) {
}

extern "C" void __ubsan_handle_type_mismatch_v1(type_mismatch_info_v1 *info, uintptr_t ptr) {
}

extern "C" void __ubsan_handle_vla_bound_not_positive(vla_bound_info *info, uintptr_t bound) {
}

extern "C" void __ubsan_handle_load_invalid_value(invalid_value_info *info, uintptr_t val) {
}

extern "C" void __ubsan_handle_builtin_unreachable(unreachable_info *info) {
}

extern "C" void __ubsan_handle_nonnull_arg(nonnull_arg_info *info) {
}

extern "C" void __ubsan_handle_type_mismatch(type_mismatch_info *info, uintptr_t ptr) {
    if (!ptr) {
        // null ptr
    } else if (ptr & (info->alignment - 1)) {
        // unaligned access
    } else {
        // oob?
    }
}