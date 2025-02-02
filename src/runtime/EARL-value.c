#include <assert.h>
#include <stdlib.h>

#include "runtime/EARL-value.h"
#include "runtime/EARL-object.h"
#include "runtime/primitives/string.h"
#include "runtime/primitives/integer.h"
#include "runtime/primitives/unit.h"
#include "misc/err.h"
#include "misc/utils.h"

EARL_value_t earl_value_integer_create(int x) {
    return (EARL_value_t) {
        .type = EARL_VALUE_TYPE_INTEGER,
        .as = {
            .integer = x
        },
        .to_cstr = earl_value_integer_to_cstr,
        .add = earl_value_integer_add,
    };
}

EARL_value_t earl_value_unit_create(void) {
    return (EARL_value_t) {
        .type = EARL_VALUE_TYPE_UNIT,
        .as = {
            .integer = 0
        },
        .to_cstr = earl_value_unit_to_cstr,
        .add = NULL,
    };
}

EARL_value_t earl_value_boolean_create(int b) {
    assert(!b || b == 1);
    return (EARL_value_t) {
        .type = EARL_VALUE_TYPE_BOOLEAN,
        .as = {
            .boolean = b,
        },
        .add = NULL,
    };
    TODO;
}

EARL_value_t earl_value_object_create(EARL_object_t *obj) {
    EARL_value_t value = (EARL_value_t) {
        .type = EARL_VALUE_TYPE_OBJECT,
        .as = {
            .obj = obj,
        },
    };

    switch (obj->type) {
    case EARL_OBJECT_TYPE_STRING: {
        value.to_cstr = earl_object_string_to_cstr;
        value.add = earl_object_string_add;
    } break;
    default: {
        err_wargs("%s: unhandled type: %d\n", __FUNCTION__, obj->type);
    } break;
    }

    return value;
}
