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

enum opcode
EVM_routines_read_byte(struct EARL_vm *vm) {
        return *vm->ip++;
}

void
EVM_routines_init(struct EARL_vm *vm) {
        vm->ip = &vm->cc->opcode.data[0];
        vm->sp = &vm->stack.data[0];
}

void
EVM_routines_stack_push(struct EARL_vm *vm, struct EARL_value *value) {
        if (vm->stack.len >= STACK_LIM)
                err("Stack Overflow");

        *vm->sp = value;
        *vm->sp++;
        vm->stack.len++;
}

struct EARL_value *
EVM_routines_stack_pop(struct EARL_vm *vm) {
        if (vm->stack.len == 0)
                err("Stack Underflow");

        *vm->sp--;
        vm->stack.len--;
        return *vm->sp;
}
