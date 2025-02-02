#include "runtime/EARL-object.h"
#include "mem/mem.h"
#include "misc/utils.h"

EARL_object_t __earl_object_create(EARL_object_type_t type) {
    return (EARL_object_t) {
        .type = type,
    };
}
