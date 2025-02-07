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

#ifndef BUILTIN_SIGS_H
#define BUILTIN_SIGS_H

#include <stddef.h>

typedef struct EARL_value EARL_value_t;

typedef EARL_value_t (*builtin_f_sig_t)(EARL_value_t *params,
                                        size_t params_len,
                                        size_t params_cap);

typedef const char *       (*to_cstr_sig_t)(const EARL_value_t *const this);
typedef       EARL_value_t (*add_sig_t)    (const EARL_value_t *this, const EARL_value_t *const other);
typedef       int          (*is_truthy_sig_t)(const EARL_value_t *const this);

// Unimplemented
typedef       EARL_value_t (*add_sub_t)    (const EARL_value_t *this, const EARL_value_t *const other);
typedef       EARL_value_t (*add_mul_t)    (const EARL_value_t *this, const EARL_value_t *const other);
typedef       EARL_value_t (*add_div_t)    (const EARL_value_t *this, const EARL_value_t *const other);
typedef       EARL_value_t (*add_mod_t)    (const EARL_value_t *this, const EARL_value_t *const other);
typedef       void         (*mutate_sig_t) (EARL_value_t *this, const EARL_value_t *const other);

#endif // BUILTIN_SIGS_H
