#pragma once
#include <stdint.h>

enum type_kinds {
    TK_INTEGER = 0x0000,
    TK_FLOAT = 0x0001,
    TK_UNKNOWN = 0xffff
};

struct source_location {
    const char *file;
    uint32_t line;
    uint32_t column;
};

struct type_descriptor {
    uint16_t kind;
    uint16_t info;
    char name[1];
};

struct type_mismatch_info {
    source_location location;
    type_descriptor *type;
    uintptr_t alignment;
    uint8_t type_check_kind;
};

struct overflow_info {
    source_location location;
    type_descriptor *type;
};

struct shift_out_of_bounds_info {
    source_location location;
    type_descriptor *lhs_type;
    type_descriptor *rhs_type;
};

struct out_of_bounds_info {
    source_location location;
    type_descriptor *array_type;
    type_descriptor *index_type;
};

struct vla_bound_info {
    source_location location;
    type_descriptor *type;
};

struct invalid_value_info {
    source_location location;
    type_descriptor *type;
};

struct unreachable_info {
    source_location location;
};

struct nonnull_arg_info {
    source_location location;
};

struct non_null_return_info {
    source_location attribute_location;
};

struct type_mismatch_info_v1 {
    source_location location;
    type_descriptor *type;
    uint8_t log_alignment;
    uint8_t type_check_kind;
};

typedef enum {
    add_overflow = 0,
    sub_overflow,
    mul_overflow,
    divrem_overflow,
    negate_overflow,
    shift_out_of_bounds,
    out_of_bounds,
    nonnull_return,
    type_mismatch_v1,
    vla_bound_not_positive,
    load_invalid_value,
    builtin_unreachable,
    nonnull_arg,
    pointer_overflow,
    type_mismatch,
} ubsan_names_t;

static const char *ubsan_type_strs[] = {
    "add_overflow:",
    "sub_overflow:",
    "mul_overflow:",
    "divrem_overflow:",
    "negate_overflow:",
    "shift_out_of_bounds:",
    "out_of_bounds:",
    "nonnull_return:",
    "type_mismatch_v1:",
    "vla_bound_not_positive:",
    "load_invalid_value:",
    "builtin_unreachable:",
    "nonnull_arg:",
    "pointer_overflow:",
    "type_mismatch:",
};
