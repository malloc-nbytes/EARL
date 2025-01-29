#ifndef BUILTINS_H
#define BUILTINS_H

// #include "s-uset.h"
#include "compiler.h"

#define BUILTIN_FUNS \
    {                \
    "println",       \
    "print",         \
    }

#define BUILTIN_VARS {}

extern const char *__builtin_funs[];
extern const char *__builtin_vars[];

extern const size_t __builtin_funs_len;
extern const size_t __builtin_vars_len;

void builtin_idents_init(cc_t *cc);

#endif // BUILTINS_H
