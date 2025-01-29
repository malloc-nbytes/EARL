#ifndef BUILTINS_H
#define BUILTINS_H

#include <stddef.h>

#include "compiler.h"
#include "EARL-value.h"

typedef void (*builtin_f_sig)(EARL_value_t **params,
                              size_t params_len,
                              size_t params_cap);

#define __BUILTIN_FUNS   \
    {                    \
        "println",       \
        "print",         \
    }

#define __BUILTIN_VARS {}

extern const char *__builtin_funs[];
extern const char *__builtin_vars[];

extern const size_t __builtin_funs_len;
extern const size_t __builtin_vars_len;

void __builtin_idents_init(cc_t *cc);

void builtin_println(EARL_value_t **params, size_t params_len, size_t params_cap);
void builtin_print(EARL_value_t **params, size_t params_len, size_t params_cap);

#endif // BUILTINS_H
