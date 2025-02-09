/** file */

// MIT License

// Copyright (c) 2025 malloc-nbytes

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "runtime/primitives/boolean.h"
#include "runtime/EARL-value.h"
#include "runtime/builtin-sigs.h"
#include "runtime/types.h"
#include "misc/utils.h"

const char *earl_value_boolean_to_cstr(const EARL_value_t *const self) {
    return self->as.boolean ? "true" : "false";
}

void earl_value_boolean_mutate(EARL_value_t *self, const EARL_value_t *const other) {
    ASSERT_TYPES_ARE_BINOP_COMPATIBLE(self->type, other->type);
    self->as.boolean = other->as.boolean;
}

int earl_value_boolean_is_truthy(const EARL_value_t *const self) {
    return self->as.boolean;
}

EARL_value_t earl_value_boolean_eq(const EARL_value_t *const self, const EARL_value_t *const other) {
    ASSERT_TYPES_ARE_BINOP_COMPATIBLE(self->type, other->type);
    return earl_value_boolean_create(self->as.boolean == other->as.boolean);
}
