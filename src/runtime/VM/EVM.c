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
#include "misc/utils.h"
#include "misc/err.h"
#include "misc/hash.h"
#include "mem/mem.h"
#include "ds/s-umap.h"

static void handle_store(EARL_vm_t *vm) {
    TODO;
}

static void handle_load_global(EARL_vm_t *vm) {
    size_t idx = vm->read_byte(vm);
    const char *sym = vm->cc->gl_syms.data[idx];
    identifier_t *identifier = s_umap_get(&vm->globals, sym);
    vm->push(vm, identifier->value);
}

static void handle_set_global(EARL_vm_t *vm) {
    size_t idx = vm->read_byte(vm);
    const char *sym = vm->cc->gl_syms.data[idx];

    identifier_t *identifier = s_umap_get(&vm->globals, sym);
    if (!identifier) {
        fprintf(stderr, "Runtime Error: Undefined global variable '%s'\n", sym);
        exit(1);
    }

    EARL_value_t value = vm->pop(vm);
    identifier->value.mutate(&identifier->value, &value);
    vm->push(vm, earl_value_unit_create());
}

static void handle_define_global(EARL_vm_t *vm) {
    size_t idx = vm->read_byte(vm);
    const char *sym = vm->cc->gl_syms.data[idx];
    EARL_value_t value = vm->pop(vm);
    identifier_t *identifier = identifier_alloc(sym, value);
    s_umap_insert(&vm->globals, sym, (uint8_t *)identifier);
    vm->push(vm, earl_value_unit_create());
}

static void handle_load(EARL_vm_t *vm) {
    TODO;
}

static void handle_call(EARL_vm_t *vm) {
    size_t args_len = vm->read_byte(vm);
    EARL_value_t callee = vm->pop(vm);

    // Builtin Function
    if (callee.type == EARL_VALUE_TYPE_BUILTIN_FUNCTION_REFERENCE) {
        builtin_f_sig_t builtin_fn = callee.as.builtin_function_reference;

        EARL_value_t *args = mem_s_malloc(args_len * sizeof(EARL_value_t), NULL, NULL);

        // Pop arguments in reverse order
        for (size_t i = 0; i < args_len; ++i)
            args[args_len-1-i] = vm->pop(vm);

        EARL_value_t res = builtin_fn(args, args_len, args_len);

        free(args);

        vm->push(vm, res);
    }

    // User defined function
    else {
        TODO;
    }
}

static void handle_add(EARL_vm_t *vm, opcode_t opc) {
    EARL_value_t n2 = vm->pop(vm);
    EARL_value_t n1 = vm->pop(vm);
    EARL_value_t res = n1.add(&n1, &n2);
    vm->push(vm, res);
}

static void handle_const(EARL_vm_t *vm) {
    size_t idx = vm->read_byte(vm);
    EARL_value_t value = vm->cc->constants.data[idx];
    vm->push(vm, value);
}

EARL_value_t EVM_exec(cc_t *cc) {
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
        case OPCODE_DEF_GLOBAL:
            handle_define_global(&vm);
            break;
        case OPCODE_LOAD_GLOBAL:
            handle_load_global(&vm);
            break;
        case OPCODE_SET_GLOBAL: {
            handle_set_global(&vm);
        } break;
        case OPCODE_LOAD: {
            handle_load(&vm);
        } break;
        case OPCODE_CALL: {
            handle_call(&vm);
        } break;
        default: {
            fprintf(stderr, "unknown opcode %d\n", opc);
            exit(1);
        }
        }

        if (b) break;
    }

    return vm.pop(&vm);
}
