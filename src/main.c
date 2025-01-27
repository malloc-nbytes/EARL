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

#include "lexer.h"
#include "utils.h"
#include "ast.h"
#include "parser.h"
#include "compiler.h"
#include "EVM.h"
#include "EARL-value.h"

#define SRC_FP "../src/input.rl"

int main(void) {
    const char *src = read_file(SRC_FP);
    lexer_t lexer = lexer_lex_src_code(src, SRC_FP);
    program_t *prog = parser_parse(&lexer);
    ast_dump(prog);

    cc_t cc = cc_compile(prog);
    cc_dump_opcode(cc);

    EARL_value_t *res = EVM_exec(&cc);
    assert(res->type == EARL_VALUE_TYPE_INTEGER);
    printf("result: %d\n", res->value.integer);

    return 0;
}
