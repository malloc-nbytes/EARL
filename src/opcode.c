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
#include <stdint.h>

#include "opcode.h"
#include "err.h"

void opcode_dump(opcode_t op) {
    switch (op) {
    case OPCODE_HALT: printf("0x%x (HALT)\n", (uint8_t)op); break;
    case OPCODE_CONST: printf("0x%x (CONST)\n", (uint8_t)op); break;
    case OPCODE_ADD: printf("0x%x (ADD)\n", (uint8_t)op); break;
    case OPCODE_MINUS: printf("0x%x (MINUS)\n", (uint8_t)op); break;
    case OPCODE_MUL: printf("0x%x (MUL)\n", (uint8_t)op); break;
    case OPCODE_DIV: printf("0x%x (DIV)\n", (uint8_t)op); break;
    case OPCODE_MOD: printf("0x%x (MOD)\n", (uint8_t)op); break;
    case OPCODE_STORE: printf("0x%x (STORE)\n", (uint8_t)op); break;
    default: {
        err_wargs("unknown opcode: 0x%x", (uint8_t)op);
    } break;
    }
}
