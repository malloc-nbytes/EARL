#ifndef STR_H
#define STR_H

#include <stddef.h>

typedef struct {
    char *data;
    size_t len;
    size_t cap;
} str_t;

str_t str_create(const char *from);

#endif // STR_H
