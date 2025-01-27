#ifndef CTX_H
#define CTX_H

#include "s-umap.h"
#include "s-uset.h"

/// @brief The context during compilation
typedef struct {
    /// @brief An array of all scoped variables
    s_uset_t *scope;

    /// @brief The length of the scope
    size_t scope_len;

    /// @brief The capacity of the scope
    size_t scope_cap;

    /// @brief An array of all scoped functions
    s_umap_t(stmt_fn_t *) *fscope;

    /// @brief The length of the fscope
    size_t fscope_len;

    /// @brief The capacity of the fscope
    size_t fscope_cap;
} ctx_t;

/// @brief Create a new context
/// @param scope_hash The hash function for the scope
/// @param scope_destr The hash function for the scope destructor
/// @param fscope_hash The hash function for the function scope
/// @param fscope_destr The hash function for the function scope destructor
ctx_t ctx_create(unsigned long (*scope_hash)(const char *),
                 unsigned long (*fscope_hash)(const char *),
                 void (*fscope_destr)(uint8_t *));

/// @brief Destroy a context
/// @param ctx The context to use
void ctx_destroy(ctx_t *ctx);

/// @brief Check if a variable is in scope
/// @param ctx The context
/// @param id The ID
/// @return 1 if it exists, 0 if otherwise
int ctx_var_in_scope(ctx_t *ctx, const char *id);

/// @brief Assert that a variable is in scope
/// @note Will panic if it does not exist
/// @param ctx The context
/// @param id The ID
void ctx_assert_var_in_scope(ctx_t *ctx, const char *id);

/// @brief Assert that a variable is not in scope
/// @note Will panic if it does exist
/// @param ctx The context
/// @param id The ID
void ctx_assert_var_not_in_scope(ctx_t *ctx, const char *id);

/// @brief Push the variable scope
/// @param ctx The context
void ctx_push_scope(ctx_t *ctx);

/// @brief Push the function scope
/// @param ctx The context
void ctx_push_fscope(ctx_t *ctx);

void ctx_add_var_to_scope(ctx_t *ctx, const char *id);

#endif // CTX_H
