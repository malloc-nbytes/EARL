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

#include "runtime/EARL-value.h"
#include "runtime/builtins.h"
#include "compiling/compiler.h"
#include "compiling/ctx.h"
#include "misc/err.h"
#include "misc/utils.h"
#include "mem/mem.h"

static int search_scope(ctx_t *ctx, const char *id) {
    for (size_t i = 0; i < ctx->scope_len; ++i)
        if (s_uset_contains(ctx->scope, id))
            return 1;
    return 0;
}

static int in_global(cc_t *cc, const char *id) {
    for (size_t i = 0; i < cc->gl_syms.len; ++i)
        if (streq(cc->gl_syms.data[i], id))
            return 1;
    return 0;
}

ctx_t ctx_create(unsigned long (*scope_hash)(const char *)) {
    ctx_t ctx = (ctx_t) {
        .scope = mem_s_malloc(sizeof(s_uset_t) * 2, NULL, NULL),
        .scope_len = 1,
        .scope_cap = 2,
    };

    ctx.scope[0] = s_uset_create(scope_hash);

    return ctx;
}

void ctx_destroy(ctx_t *ctx) {
    for (size_t i = 0; i < ctx->scope_len; ++i)
        s_uset_destroy(&ctx->scope[i]);

    ctx->scope_len = ctx->scope_cap = 0;
}

int ctx_var_in_scope(ctx_t *ctx, cc_t *cc, const char *id) {
    return in_global(cc, id) || search_scope(ctx, id);
    return 0;
}

void ctx_assert_var_in_scope(ctx_t *ctx, cc_t *cc, const char *id) {
    if (in_global(cc, id) || search_scope(ctx, id))
        return;
    err_wargs("identifier `%s` was not defined", id);
}

void ctx_assert_var_not_in_scope(ctx_t *ctx, cc_t *cc, const char *id) {
    if (in_global(cc, id) || search_scope(ctx, id))
        err_wargs("identifier `%s` is already defined", id);
}

void ctx_push_scope(ctx_t *ctx) {
    s_uset_t set = s_uset_create(ctx->scope[0].hash);
    da_append(ctx->scope, ctx->scope_len, ctx->scope_cap, s_uset_t *, set);
}

void ctx_add_var_to_scope(ctx_t *ctx, const char *id) {
    s_uset_insert(&ctx->scope[ctx->scope_len - 1], id);
}
