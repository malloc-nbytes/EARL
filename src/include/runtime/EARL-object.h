#ifndef EARL_OBJECT_H
#define EARL_OBJECT_H

#include <stddef.h>

#include "runtime/EARL-value.h"

typedef enum {
    EARL_OBJECT_TYPE_STRING = 0,
    __EARL_OBJECT_TYPE_LEN, // Used in types.c
} EARL_object_type_t;

typedef struct EARL_object {
    EARL_object_type_t type;
} EARL_object_t;

typedef struct EARL_object_string {
    EARL_object_t base;
    char *chars;
    size_t len;
    size_t cap;
} EARL_object_string_t;

EARL_object_t __earl_object_create(EARL_object_type_t type);

#endif // EARL_OBJECT_H
