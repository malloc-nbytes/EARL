#ifndef STRING_H
#define STRING_H

EARL_object_string_t *earl_object_string_alloc(const char *chars);
const char *earl_object_string_to_cstr(const EARL_value_t *const value);
EARL_value_t earl_object_string_add(const EARL_value_t *this, const EARL_value_t *const other);

#endif // STRING_H
