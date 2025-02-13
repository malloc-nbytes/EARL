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
#include "runtime/EARL-object.h"
#include "runtime/identifier.h"
#include "misc/utils.h"
#include "misc/err.h"
#include "misc/hash.h"
#include "mem/mem.h"
#include "ds/s-umap.h"

#define GET_IDENTIFIER_NAME_FROM_CONSTANT_POOL(vm, idx) ({              \
            EARL_value_t __symbol_ = (vm)->constants.data[idx];         \
            assert(__symbol_.type == EARL_VALUE_TYPE_OBJECT);           \
            EARL_object_t *__obj_ = __symbol_.as.obj;                   \
            assert(__obj_->type == EARL_OBJECT_TYPE_STRING);            \
            ((EARL_object_string_t *)__obj_)->chars;                    \
})

#define READ_SHORT(vm) (vm->ip += 2, (uint16_t)((vm->ip[-2] << 8) | vm->ip[-1]))

static void handle_load_global(EARL_vm_t *vm) {
    size_t        idx   = vm->read_byte(vm);
    const char   *name  = GET_IDENTIFIER_NAME_FROM_CONSTANT_POOL(vm, idx);
    identifier_t *ident = (identifier_t *)s_umap_get(&vm->globals, name);

    vm->push(vm, ident->value);
}

static void handle_set_global(EARL_vm_t *vm) {
    size_t        idx        = vm->read_byte(vm);
    const char   *name       = GET_IDENTIFIER_NAME_FROM_CONSTANT_POOL(vm, idx);
    identifier_t *identifier = (identifier_t *)s_umap_get(&vm->globals, name);

    if (!identifier) {
        fprintf(stderr, "Runtime Error: Undefined global variable '%s'\n", name);
        exit(1);
    }

    EARL_value_t value = vm->pop(vm);
    identifier->value.mutate(&identifier->value, &value);
}

static void handle_define_global(EARL_vm_t *vm) {
    size_t        idx   = vm->read_byte(vm);
    EARL_value_t  value = vm->pop(vm);

    const char   *name  = GET_IDENTIFIER_NAME_FROM_CONSTANT_POOL(vm, idx);
    identifier_t *ident = identifier_alloc(name, value);

    s_umap_insert(&vm->globals, name, (void *)ident);
}

// Caller must free()
static EARL_value_t *gather_function_parameters(EARL_vm_t *vm, size_t args_len) {
    EARL_value_t *args = (EARL_value_t *)mem_s_malloc(args_len * sizeof(EARL_value_t), NULL, NULL);

    // Pop arguments in reverse order
    for (size_t i = 0; i < args_len; ++i)
        args[args_len-1-i] = vm->pop(vm);

    return args;
}

static void handle_call(EARL_vm_t *vm) {
    size_t args_len = vm->read_byte(vm);
    EARL_value_t callee = vm->pop(vm);

    // Builtin Function
    if (callee.type == EARL_VALUE_TYPE_BUILTIN_FUNCTION_REFERENCE) {
        builtin_f_sig_t builtin_fn = callee.as.builtin_function_reference;
        EARL_value_t *args = gather_function_parameters(vm, args_len);
        EARL_value_t res = builtin_fn(args, args_len);
        free(args);
        vm->push(vm, res);
    }

    // User defined function
    else {
        TODO;
    }
}

static void handle_binop(EARL_vm_t *vm, uint8_t opc) {
    EARL_value_t
        n2 = vm->pop(vm),
        n1 = vm->pop(vm);

    EARL_value_t res;

    switch (opc) {
    case OPCODE_ADD: res = n1.add(&n1, &n2); break;
    case OPCODE_SUB: res = n1.sub(&n1, &n2); break;
    case OPCODE_MUL: res = n1.mul(&n1, &n2); break;
    case OPCODE_DIV: res = n1.div(&n1, &n2); break;
    case OPCODE_MOD: res = n1.mod(&n1, &n2); break;
    case OPCODE_EQ:
    case OPCODE_NEQ:
    case OPCODE_LT:
    case OPCODE_GT:
    case OPCODE_LE:
    case OPCODE_GE:  res = n1.cmp(&n1, &n2, opc); break;
    default:         err_wargs("runtime: unimplemented binop: %d", (int)opc);
    }

    vm->push(vm, res);
}

static void handle_const(EARL_vm_t *vm) {
    size_t idx = vm->read_byte(vm);
    EARL_value_t value = vm->constants.data[idx];
    vm->push(vm, value);
}

static void handle_pop(EARL_vm_t *vm) {
    (void)vm->pop(vm);
}

static void handle_load_local(EARL_vm_t *vm) {
    size_t slot = vm->read_byte(vm);
    vm->push(vm, vm->stack.data[slot]);
}

static void handle_set_local(EARL_vm_t *vm) {
    size_t slot = vm->read_byte(vm);
    EARL_value_t
        value = vm->pop(vm),
        *var = &vm->stack.data[slot];
    var->mutate(var, &value);
}

static void handle_def_local(EARL_vm_t *vm) {
    size_t slot = vm->read_byte(vm);
    EARL_value_t value = vm->pop(vm);
    vm->stack.data[slot] = value;
}

static void handle_jump_if_false(EARL_vm_t *vm) {
    uint16_t offset = READ_SHORT(vm);
    EARL_value_t condition = vm->peek(vm, 0);
    if (!condition.is_truthy(&condition))
        vm->ip += offset;
}

static void handle_jump_if_true(EARL_vm_t *vm) {
    uint16_t offset = READ_SHORT(vm);
    EARL_value_t condition = vm->peek(vm, 0);
    if (condition.is_truthy(&condition))
        vm->ip += offset;
}

static void handle_jump(EARL_vm_t *vm) {
    uint16_t offset = READ_SHORT(vm);
    vm->ip += offset;
}

static void handle_loop(EARL_vm_t *vm) {
    uint16_t offset = READ_SHORT(vm);
    vm->ip -= offset;
    //(void)vm->pop(vm);
}

void EVM_exec(EARL_object_function_t *fun,
              EARL_value_t *constants,
              size_t constants_len) {
    EARL_vm_t vm = (EARL_vm_t) {
        .stack      = { .len = 0 },
        .globals    = s_umap_create(djb2, NULL),
        .constants  = { .data = constants, .len = constants_len },
        .fun        = fun,
        .sp         = NULL,
        .ip         = NULL,
        .read_byte  = EVM_routines_read_byte,
        .init       = EVM_routines_init,
        .push       = EVM_routines_stack_push,
        .pop        = EVM_routines_stack_pop,
        .peek       = EVM_routines_stack_peek,
        .dump_stack = EVM_routines_dump_stack,
    };

    vm.init(&vm);

    // Begin interpretation
    while (1) {
        int b = 0;

        uint8_t opc;

        switch (opc = vm.read_byte(&vm)) {
        case OPCODE_HALT:
            b = 1;
            break;
        case OPCODE_CONST:
            handle_const(&vm);
            break;
        case OPCODE_POP:
            handle_pop(&vm);
            break;
        case OPCODE_SUB:
        case OPCODE_MUL:
        case OPCODE_DIV:
        case OPCODE_MOD:
        case OPCODE_ADD:
        case OPCODE_EQ:
        case OPCODE_NEQ:
        case OPCODE_LT:
        case OPCODE_GT:
        case OPCODE_LE:
        case OPCODE_GE:
            handle_binop(&vm, opc);
            break;
        case OPCODE_DEF_GLOBAL:
            handle_define_global(&vm);
            break;
        case OPCODE_LOAD_GLOBAL:
            handle_load_global(&vm);
            break;
        case OPCODE_SET_GLOBAL:
            handle_set_global(&vm);
            break;
        case OPCODE_DEF_LOCAL:
            handle_def_local(&vm);
            break;
        case OPCODE_SET_LOCAL:
            handle_set_local(&vm);
            break;
        case OPCODE_LOAD_LOCAL:
            handle_load_local(&vm);
            break;
        case OPCODE_CALL:
            handle_call(&vm);
            break;
        case OPCODE_JUMP_IF_FALSE:
            handle_jump_if_false(&vm);
            break;
        case OPCODE_JUMP_IF_TRUE:
            handle_jump_if_true(&vm);
            break;
        case OPCODE_JUMP:
            handle_jump(&vm);
            break;
        case OPCODE_LOOP:
            handle_loop(&vm);
            break;
        default: {
            fprintf(stderr, "unknown opcode %d\n", opc);
            exit(1);
        }
        }

        if (b) break;
    }
}
