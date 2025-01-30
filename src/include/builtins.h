#ifndef BUILTINS_H
#define BUILTINS_H

#include <stddef.h>

#include "compiler.h"
#include "s-umap.h"
#include "EARL-value.h"

typedef void (*builtin_f_sig_t)(EARL_value_t **params,
                                size_t params_len,
                                size_t params_cap);

#define __BUILTIN_FUNS   \
    {                    \
        "println",       \
        "print",         \
    }

#define __BUILTIN_VARS {}

extern const char *__builtin_function_identifiers[];
extern const char *__builtin_variable_identifiers[];

extern const size_t __builtin_function_identifiers_len;
extern const size_t __builtin_variable_identifiers_len;

extern s_umap_t(builtin_f_sig_t) builtin_funs;

void __builtin_idents_init(cc_t *cc);

void builtin_println(EARL_value_t **params, size_t params_len, size_t params_cap);
void builtin_print(EARL_value_t **params, size_t params_len, size_t params_cap);

#endif // BUILTINS_H
