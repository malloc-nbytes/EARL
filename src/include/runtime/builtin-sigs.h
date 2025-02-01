#ifndef BUILTIN_SIGS_H
#define BUILTIN_SIGS_H

typedef struct EARL_value EARL_value_t;

typedef EARL_value_t *(*builtin_f_sig_t)(EARL_value_t **params,
                                         size_t params_len,
                                         size_t params_cap);

typedef const char *(*to_cstr_sig_t)(const EARL_value_t *const value);
typedef       int   (*boolean_sig_t)(const EARL_value_t *const value);
typedef       void  (*mutate_sig_t) (const EARL_value_t *value, const EARL_value_t *const other);

#endif // BUILTIN_SIGS_H
