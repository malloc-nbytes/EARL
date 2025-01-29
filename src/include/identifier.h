#ifndef IDENTIFIER_H
#define IDENTIFIER_H

#include <stdint.h>

#include "EARL-value.h"

typedef struct {
    const char *id;
    EARL_value_t *value;
    uint8_t refc;
} identifier_t;

identifier_t *identifier_alloc(const char *id, EARL_value_t *value);

#endif // IDENTIFIER_H
