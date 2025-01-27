#ifndef EARL_VALUE_H
#define EARL_VALUE_H

typedef enum {
    EARL_VALUE_TYPE_UNIT,
    EARL_VALUE_TYPE_INTEGER,
} EARL_value_type_t;

typedef struct {
    EARL_value_type_t type;
    union {
        void *unit;
        int integer;
    } value;
} EARL_value_t;

/// @brief Allocate a new EARL value
/// @param type The type of the value
/// @param data The C data
/// @return The allocated EARL value
EARL_value_t *EARL_value_alloc(EARL_value_type_t type, void *data);

#endif // EARL_VALUE_H
