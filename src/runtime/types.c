#include "runtime/types.h"
#include "runtime/EARL-value.h"
#include "runtime/EARL-object.h"

// Value types
static EARL_value_type_t types_simple_compat_types_unit[]    = {EARL_VALUE_TYPE_UNIT};
static EARL_value_type_t types_simple_compat_types_integer[] = {EARL_VALUE_TYPE_INTEGER};
static EARL_value_type_t types_simple_compat_types_boolean[] = {EARL_VALUE_TYPE_BOOLEAN};

static EARL_value_type_t *types_simple_compat_types[] = {
    types_simple_compat_types_unit,
    types_simple_compat_types_integer,
    types_simple_compat_types_boolean,
};

// Object types
static EARL_object_type_t types_object_compat_types_string[] = {EARL_OBJECT_TYPE_STRING};

static EARL_object_type_t *types_object_compat_types[] = {
    types_object_compat_types_string,
};

int types_are_compatible(EARL_value_type_t ty1, EARL_value_type_t ty2) {
    _Static_assert(sizeof(types_simple_compat_types)/sizeof(*types_simple_compat_types) == __EARL_VALUE_TYPE_LEN,
                   "EARL value compatible types is incomplete");
    _Static_assert(sizeof(types_object_compat_types)/sizeof(*types_object_compat_types) == __EARL_OBJECT_TYPE_LEN,
                   "EARL object compatible types is incomplete");

    if (ty1 == EARL_VALUE_TYPE_OBJECT) {
        EARL_object_type_t *types = types_object_compat_types[ty1];
        for (size_t i = 0; i < sizeof(types)/sizeof(*types); ++i)
            if (types[i] == ty2)
                return 1;
    } else {
        EARL_value_type_t *types = types_simple_compat_types[ty1];
        for (size_t i = 0; i < sizeof(types)/sizeof(*types); ++i)
            if (types[i] == ty2)
                return 1;
    }
    return 0;
}
