#include "runtime/primitives/builtin-function-reference.h"
#include "runtime/EARL-value.h"
#include "runtime/builtin-sigs.h"

const char *earl_value_builtin_function_reference_to_cstr(const EARL_value_t *const _) {
    return "<Builtin-Function-Reference>";
}
