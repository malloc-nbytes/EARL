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
#include <stdio.h>

#include "runtime/VM/EVM-routines.h"
#include "runtime/EARL-value.h"
#include "runtime/builtins.h"
#include "runtime/identifier.h"
#include "ds/s-umap.h"
#include "misc/err.h"
#include "misc/utils.h"
#include "misc/hash.h"

uint8_t EVM_routines_read_byte(EARL_vm_t *vm) {
    return *vm->ip++;
}

void EVM_routines_init(EARL_vm_t *vm) {
    vm->ip = &vm->fun->opcode.data[0];
    vm->sp = vm->stack.data;

    vm->globals = s_umap_create(djb2, NULL);

    for (size_t i = 0; i < __builtin_function_identifiers_len; ++i) {
        builtin_f_sig_t fun = builtin_funs[i];
        //builtin_f_sig_t fun = s_umap_get(&builtin_funs, __builtin_function_identifiers[i]);

        EARL_value_t value
            = earl_value_builtin_function_reference_create(fun);

        identifier_t *var = identifier_alloc(__builtin_function_identifiers[i], value);
        s_umap_insert(&vm->globals, var->id, (void *)var);
    }
    /* for (size_t i = 0; i < __builtin_variable_identifiers_len; ++i) { */
    /*     TODO; */
    /* } */
}

EARL_value_t EVM_routines_stack_peek(EARL_vm_t *vm, size_t pos) {
    if (pos >= vm->stack.len)
        err_wargs("stack underflow: cannot peek at distance %zu", pos);
    return vm->stack.data[vm->stack.len - 1 - pos];
}

void EVM_routines_stack_push(EARL_vm_t *vm, EARL_value_t value) {
    if (vm->stack.len >= STACK_LIM)
        err("Stack Overflow");

    *vm->sp = value;
    vm->sp++;
    vm->stack.len++;
}

EARL_value_t EVM_routines_stack_pop(EARL_vm_t *vm) {
    if (vm->stack.len == 0)
        err("Stack Underflow");

    vm->sp--;
    vm->stack.len--;
    return *vm->sp;
}

void EVM_routines_dump_stack(EARL_vm_t *vm) {
    printf("Dumping stack\n");
    for (size_t i = 0; i < vm->stack.len; ++i) {
        printf("Stack[%zu]: ", i);
        // EARL_value_dump(vm->stack.data[i]);
        TODO;
    }
}
