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

#include "runtime/VM/opcode.h"
#include "compiling/compiler.h"
#include "misc/err.h"

static int simple_instruction(const char *name, int offset) {
    printf("%s\n", name);
    return offset+1;
}

int disassemble_instruction(cc_t *cc, int offset) {
    printf("%04d ", offset);

    opcode_t instr = cc->opcode.data[offset];
    switch (instr) {
    case OPCODE_HALT:
        return simple_instruction("OPCODE_HALT", offset);
    default: {
        printf("%s: unknown opcode: 0x%x", __FUNCTION__, (uint8_t)instr);
        return offset+1;
    } break;
    }
}

void opcode_dump(opcode_t op) {
}
