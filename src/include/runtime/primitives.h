#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "EARL-value.h"

////////////////////////////////////
// INTEGER
////////////////////////////////////
const char *int_to_cstr(const EARL_value_t *const value);

////////////////////////////////////
// UNIT
////////////////////////////////////
const char *unit_to_cstr(const EARL_value_t *const value);

////////////////////////////////////
// BUILTIN FUNCTION REFERENCE
////////////////////////////////////
const char *builtin_function_reference_to_cstr(const EARL_value_t *const value);

#endif // PRIMITIVES_H
