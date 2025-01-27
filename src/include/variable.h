#ifndef VARIABLE_H
#define VARIABLE_H

#include <stdint.h>

#include "EARL-value.h"

typedef struct {
    const char *id;
    EARL_value_t *value;
    uint8_t refc;
} variable_t;

variable_t *variable_alloc(const char *id, EARL_value_t *value);

#endif // VARIABLE_H
