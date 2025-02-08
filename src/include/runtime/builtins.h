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

#ifndef BUILTINS_H
#define BUILTINS_H

#include <stddef.h>

#include "compiling/compiler.h"
#include "runtime/EARL-value.h"
#include "runtime/builtin-sigs.h"
#include "ds/s-umap.h"

#define __BUILTIN_FUNS   \
    {                    \
        "println",       \
        "print",         \
    }

#define __BUILTIN_VARS {}

#define BUILTIN_FUNS_LIM 32

extern const char *__builtin_function_identifiers[];
extern const char *__builtin_variable_identifiers[];

extern const size_t __builtin_function_identifiers_len;
extern const size_t __builtin_variable_identifiers_len;

//extern s_umap_t(builtin_f_sig_t) builtin_funs;
extern builtin_f_sig_t builtin_funs[BUILTIN_FUNS_LIM];

void __builtin_idents_init(cc_t *cc);

EARL_value_t builtin_println(EARL_value_t *params, size_t params_len);
EARL_value_t builtin_print(EARL_value_t *params, size_t params_len);

#endif // BUILTINS_H
