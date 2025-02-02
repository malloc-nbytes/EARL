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

#include <assert.h>
#include <stdio.h>

#include "compiling/compiler.h"
#include "runtime/builtins.h"
#include "misc/hash.h"
#include "misc/utils.h"

const char *__builtin_function_identifiers[] = __BUILTIN_FUNS;
const char *__builtin_variable_identifiers[] = __BUILTIN_VARS;

const size_t __builtin_function_identifiers_len
    = sizeof(__builtin_function_identifiers) / sizeof(*__builtin_function_identifiers);
const size_t __builtin_variable_identifiers_len
    = sizeof(__builtin_variable_identifiers) / sizeof(*__builtin_variable_identifiers);

s_umap_t(builtin_f_sig_t) builtin_funs;

EARL_value_t *builtin_println(EARL_value_t **params, size_t params_len, size_t params_cap) {
    /* for (size_t i = 0; i < params_len; ++i) */
    /*     printf("%s", params[i]->to_cstr(params[i])); */
    /* putchar('\n'); */
    /* fflush(stdout); */
    /* return EARL_value_alloc(EARL_VALUE_TYPE_UNIT, NULL); */
    TODO;
}

EARL_value_t *builtin_print(EARL_value_t **params, size_t params_len, size_t params_cap) {
    TODO;
}

static void fill_builtin_c_functions(void) {
    builtin_funs = s_umap_create(djb2, NULL);

    s_umap_insert(&builtin_funs, "println", (uint8_t *)builtin_println);
    s_umap_insert(&builtin_funs, "print",   (uint8_t *)builtin_print);
}

void __builtin_idents_init(cc_t *cc) {
    TODO;
    for (size_t i = 0; i < __builtin_function_identifiers_len; ++i)
        //(void)cc_push_global(cc, __builtin_function_identifiers[i]);

    for (size_t i = 0; i < __builtin_variable_identifiers_len; ++i)
        //(void)cc_push_global(cc, __builtin_variable_identifiers[i]);

    fill_builtin_c_functions();
}
