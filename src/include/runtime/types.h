#ifndef TYPES_H
#define TYPES_H

#include "runtime/EARL-value.h"
#include "runtime/EARL-object.h"
#include "misc/err.h"

#define ASSERT_TYPES_ARE_BINOP_COMPATIBLE(ty1, ty2)                                          \
    do {                                                                                     \
        if (!types_are_compatible(ty1, ty2))                                                 \
            err_wargs("%s: type %d is not compatible with type %d", __FUNCTION__, ty1, ty2); \
    } while (0)

int types_are_compatible(EARL_value_type_t ty1, EARL_value_type_t ty2);

#endif // TYPES_H
