#ifndef FUNCTION_H
#define FUNCTION_H

#include <stddef.h>

#include "runtime/EARL-object.h"
#include "runtime/VM/opcode.h"

EARL_object_function_t *earl_object_function_alloc(size_t arity, opcode_t opcode, const char *id);
const char *            earl_object_function_to_cstr(const EARL_value_t *const value);

#endif // FUNCTION_H
