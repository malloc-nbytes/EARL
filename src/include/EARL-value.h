#ifndef EARL_VALUE_H
#define EARL_VALUE_H

typedef enum {
    EARL_VALUE_TYPE_UNIT,
    EARL_VALUE_TYPE_INTEGER,
    EARL_VALUE_TYPE_FUNCTION_REFERENCE,
} EARL_value_type_t;

typedef struct {
} EARL_value_function_reference_t;

typedef struct {
    EARL_value_type_t type;
    union {
        void *unit;
        int integer;
        EARL_value_function_reference_t *fref;
    } actual;
} EARL_value_t;

/// @brief Allocate a new EARL value
/// @param type The type of the value
/// @param data The C data
/// @return The allocated EARL value
EARL_value_t *EARL_value_alloc(EARL_value_type_t type, void *data);

/// @brief Debug print an EARL runtime value
/// @param value The value to print
void EARL_value_dump(const EARL_value_t *value);

#endif // EARL_VALUE_H
