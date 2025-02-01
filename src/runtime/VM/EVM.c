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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "runtime/VM/EVM.h"
#include "runtime/VM/EVM-routines.h"
#include "runtime/VM/opcode.h"
#include "runtime/EARL-value.h"
#include "runtime/identifier.h"
#include "ds/s-umap.h"
#include "misc/utils.h"
#include "misc/err.h"
#include "misc/hash.h"

static void handle_store(EARL_vm_t *vm) {
    size_t idx = vm->read_byte(vm);

    const char *symbol = vm->cc->gl_syms.data[idx];
    assert(symbol && "Symbol must exist in the global symbol table");

    EARL_value_t *value = vm->pop(vm);
    identifier_t *var = identifier_alloc(symbol, value);
    s_umap_insert(&vm->globals, symbol, (uint8_t *)var);

    vm->push(vm, EARL_value_alloc(EARL_VALUE_TYPE_UNIT, NULL));
}

static void handle_load(EARL_vm_t *vm) {
    size_t idx = vm->read_byte(vm);
    const char *id = vm->cc->gl_syms.data[idx];
    identifier_t *var = (identifier_t *)s_umap_get(&vm->globals, id);

    if (!var) {
        fprintf(stderr, "Runtime Error: Undefined identifier '%s'\n", id);
        exit(1);
    }

    EARL_value_t *value = var->value;

    vm->push(vm, value);
}

static void handle_call(EARL_vm_t *vm) {
    size_t args_len = vm->read_byte(vm);  // Read the number of arguments

    EARL_value_t *func_value = vm->pop(vm);  // Pop function reference

    if (func_value->type == EARL_VALUE_TYPE_FUNCTION_REFERENCE) {
        TODO;
        /* stmt_fn_t *fn = func_value->actual.fn; */

        /* EARL_value_t **args = malloc(args_len * sizeof(EARL_value_t *)); */
        /* if (!args) { */
        /*     fprintf(stderr, "Memory allocation failed in handle_call\n"); */
        /*     exit(1); */
        /* } */

        /* for (size_t i = 0; i < args_len; ++i) */
        /*     args[args_len - 1 - i] = vm->pop(vm); */

        /* EARL_value_t *result = fn(vm, args, args_len); */

        /* free(args); */

        /* vm->push(vm, result ? result : EARL_value_alloc(EARL_VALUE_TYPE_UNIT, NULL)); */
    } else if (func_value->type == EARL_VALUE_TYPE_BUILTIN_FUNCTION_REFERENCE) {
        builtin_f_sig_t builtin_fn = func_value->actual.builtin_fun;

        EARL_value_t **args = malloc(args_len * sizeof(EARL_value_t *));
        if (!args) {
            fprintf(stderr, "Memory allocation failed in handle_call\n");
            exit(1);
        }

        // Pop arguments in reverse order
        for (size_t i = 0; i < args_len; ++i)
            args[args_len - 1 - i] = vm->pop(vm);

        EARL_value_t *res = builtin_fn(args, args_len, args_len);

        free(args);

        vm->push(vm, res);

    } else {
        fprintf(stderr, "Runtime Error: Attempted to call a non-function value\n");
        exit(1);
    }
}

static void handle_add(EARL_vm_t *vm, opcode_t opc) {
    EARL_value_t *n1 = vm->pop(vm);
    EARL_value_t *n2 = vm->pop(vm);

    int v1 = n1->actual.integer,
        v2 = n2->actual.integer;

    int res = opc == OPCODE_ADD ? v2 + v1
        : opc == OPCODE_MINUS ? v2 - v1
        : opc == OPCODE_MUL ? v2 * v1
        : opc == OPCODE_DIV ? v2 / v1
        : v2 % v1;

    EARL_value_t *res_value = EARL_value_alloc(EARL_VALUE_TYPE_INTEGER, &res);
    vm->push(vm, res_value);
}

static void handle_const(EARL_vm_t *vm) {
    size_t idx = vm->read_byte(vm);
    EARL_value_t *constant = vm->cc->const_pool.data[idx];
    vm->push(vm, constant);
}

EARL_value_t *EVM_exec(cc_t *cc) {
    printf("Begin Interpreter...\n");

    EARL_vm_t vm = (EARL_vm_t) {
        .stack      = { .len = 0 },
        .globals    = s_umap_create(djb2, NULL),
        .cc         = cc,
        .sp         = NULL,
        .ip         = NULL,

        .read_byte  = EVM_routines_read_byte,
        .init       = EVM_routines_init,
        .push       = EVM_routines_stack_push,
        .pop        = EVM_routines_stack_pop,
        .dump_stack = EVM_routines_dump_stack,
    };

    vm.init(&vm);

    // Begin interpretation
    while (1) {
        int b = 0;

        opcode_t opc;

        switch (opc = vm.read_byte(&vm)) {
        case OPCODE_HALT:
            b = 1;
            break;
        case OPCODE_CONST:
            handle_const(&vm);
            break;
        case OPCODE_MINUS:
        case OPCODE_MUL:
        case OPCODE_DIV:
        case OPCODE_ADD:
            handle_add(&vm, opc);
            break;
        case OPCODE_STORE:
            handle_store(&vm);
            break;
        case OPCODE_LOAD: {
            handle_load(&vm);
        } break;
        case OPCODE_CALL: {
            handle_call(&vm);
        } break;
        default: {
            fprintf(stderr, "unknown opcode %#x\n", opc);
            exit(1);
        }
        }

        if (b) break;
    }

    return vm.pop(&vm);
}
