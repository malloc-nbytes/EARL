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

#include "EVM-routines.h"
#include "err.h"

opcode_t EVM_routines_read_byte(EARL_vm_t *vm) {
    return *vm->ip++;
}

void EVM_routines_init(EARL_vm_t *vm) {
    vm->ip = &vm->cc->opcode.data[0];
    vm->sp = &vm->stack.data[0];
}

void EVM_routines_stack_push(EARL_vm_t *vm, EARL_value_t *value) {
    if (vm->stack.len >= STACK_LIM)
        err("Stack Overflow");

    *vm->sp = value;
    *vm->sp++;
    vm->stack.len++;
}

EARL_value_t *EVM_routines_stack_pop(EARL_vm_t *vm) {
    if (vm->stack.len == 0)
        err("Stack Underflow");

    *vm->sp--;
    vm->stack.len--;
    return *vm->sp;
}

void EVM_routines_dump_stack(EARL_vm_t *vm) {
    printf("Dumping stack\n");
    for (size_t i = 0; i < vm->stack.len; ++i) {
        printf("Stack[%zu]: ", i);
        EARL_value_dump(vm->stack.data[i]);
    }
}
