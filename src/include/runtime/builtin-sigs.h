#ifndef BUILTIN_SIGS_H
#define BUILTIN_SIGS_H

#include <stddef.h>

typedef struct EARL_value EARL_value_t;

typedef EARL_value_t (*builtin_f_sig_t)(EARL_value_t *params,
                                        size_t params_len,
                                        size_t params_cap);

typedef const char *       (*to_cstr_sig_t)(const EARL_value_t *const this);
typedef       EARL_value_t (*add_sig_t)    (const EARL_value_t *this, const EARL_value_t *const other);

// Unimplemented
typedef       EARL_value_t (*add_sub_t)    (const EARL_value_t *this, const EARL_value_t *const other);
typedef       EARL_value_t (*add_mul_t)    (const EARL_value_t *this, const EARL_value_t *const other);
typedef       EARL_value_t (*add_div_t)    (const EARL_value_t *this, const EARL_value_t *const other);
typedef       EARL_value_t (*add_mod_t)    (const EARL_value_t *this, const EARL_value_t *const other);
typedef       int          (*boolean_sig_t)(const EARL_value_t *const this);
typedef       void         (*mutate_sig_t) (EARL_value_t *this, const EARL_value_t *const other);

#endif // BUILTIN_SIGS_H
