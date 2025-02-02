#ifndef INTEGER_H
#define INTEGER_H

#include "runtime/EARL-value.h"

const char * earl_value_integer_to_cstr(const EARL_value_t *const this);
EARL_value_t earl_value_integer_add(const EARL_value_t *this, const EARL_value_t *const other);
void         earl_value_integer_mutate(EARL_value_t *this, const EARL_value_t *const other);

#endif // INTEGER_H
