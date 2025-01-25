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

#include <stdio.h>
#include <stdlib.h>

#include "earl-value.h"
#include "ctx.h"
#include "err.h"
#include "utils.h"

struct ctx
ctx_create(unsigned long (*scope_hash)(const char *),
           unsigned long (*fscope_hash)(const char *),
           void (*fscope_destr)(uint8_t *)) {
        struct ctx ctx = (struct ctx) {
                .scope = utils_s_malloc(sizeof(struct s_uset) * 2, NULL, NULL),
                .scope_len = 1, .scope_cap = 2,
                .fscope = utils_s_malloc(sizeof(struct s_umap) * 2, NULL, NULL),
                .fscope_len = 1, .fscope_cap = 2,
        };

        ctx.scope[0] = s_uset_create(scope_hash);
        ctx.fscope[0] = s_umap_create(fscope_hash, fscope_destr);

        return ctx;
}

void
ctx_destroy(struct ctx *ctx) {
        for (size_t i = 0; i < ctx->scope_len; ++i)
                s_uset_destroy(&ctx->scope[i]);

        for (size_t i = 0; i < ctx->scope_len; ++i)
                s_umap_destroy(&ctx->fscope[i]);

        ctx->scope_len = ctx->scope_cap = 0;
        ctx->fscope_len = ctx->fscope_cap = 0;
}

int
ctx_var_in_scope(struct ctx *ctx, const char *id) {
        for (size_t i = 0; i < ctx->scope_len; ++i)
                if (s_uset_contains(&ctx->scope[i], id))
                        return 1;
        return 0;
}

void
ctx_assert_var_in_scope(struct ctx *ctx, const char *id) {
        for (size_t i = 0; i < ctx->scope_len; ++i)
                if (s_uset_contains(&ctx->scope[i], id))
                        return;
        err_wargs("variable `%s` was not defined", id);
}

void
ctx_assert_var_not_in_scope(struct ctx *ctx, const char *id) {
        for (size_t i = 0; i < ctx->scope_len; ++i)
                if (s_uset_contains(&ctx->scope[i], id))
                        err_wargs("variable `%s` is already defined", id);
}

void
ctx_push_scope(struct ctx *ctx) {
        struct s_uset set = s_uset_create(ctx->scope[0].hash);
        utils_da_append(ctx->scope, ctx->scope_len, ctx->scope_cap, struct s_uset *, set);
}

void
ctx_push_fscope(struct ctx *ctx) {
        struct s_umap map = s_umap_create(ctx->fscope[0].hash, ctx->fscope[0].destroy_value);
        utils_da_append(ctx->fscope, ctx->fscope_len, ctx->fscope_cap, struct s_umap *, map);
}

void
ctx_add_var_to_scope(struct ctx *ctx, const char *id) {
        s_uset_insert(&ctx->scope[ctx->scope_len - 1], id);
}
