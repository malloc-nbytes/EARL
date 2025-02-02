#include <stdio.h>

#include "runtime/primitives/integer.h"
#include "runtime/types.h"
#include "mem/mem.h"

const char *earl_value_integer_to_cstr(const EARL_value_t *const this) {
    char *buffer = mem_s_malloc(20, NULL, NULL);
    snprintf(buffer, 20, "%d", this->as.integer);
    return buffer;
}

EARL_value_t earl_value_integer_add(const EARL_value_t *this,
                                    const EARL_value_t *const other) {
    ASSERT_TYPES_ARE_BINOP_COMPATIBLE(this->type, other->type);
    int sum = this->as.integer + other->as.integer;
    return earl_value_integer_create(sum);
}

void earl_value_integer_mutate(EARL_value_t *this, const EARL_value_t *const other) {
    ASSERT_TYPES_ARE_BINOP_COMPATIBLE(this->type, other->type);
    this->as.integer = other->as.integer;
}
