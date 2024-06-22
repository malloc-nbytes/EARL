/** @file */

// MIT License

// Copyright (c) 2023 malloc-nbytes

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

#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>

/**
 * A few things that are useful throughout the entire project.
 */

#define COMMON_EARLATTR_WORLD "world"
#define COMMON_EARLATTR_PUB   "pub"
#define COMMON_EARLATTR_REF   "ref"

enum class Attr {
    World = 1 << 0,
    Pub = 1 << 1,
    Ref = 1 << 2,
};

// Keywords
#define COMMON_EARLKW_LET    "let"
#define COMMON_EARLKW_FN     "fn"
#define COMMON_EARLKW_RETURN "return"
#define COMMON_EARLKW_IF     "if"
#define COMMON_EARLKW_ELSE   "else"
#define COMMON_EARLKW_WHILE  "while"
#define COMMON_EARLKW_FOR    "for"
#define COMMON_EARLKW_IN     "in"
#define COMMON_EARLKW_IMPORT "import"
#define COMMON_EARLKW_MODULE "mod"
#define COMMON_EARLKW_ASCPL {COMMON_EARLKW_LET, COMMON_EARLKW_FN, COMMON_EARLKW_RETURN, COMMON_EARLKW_IF, COMMON_EARLKW_ELSE, COMMON_EARLKW_WHILE, COMMON_EARLKW_FOR, COMMON_EARLKW_IN, COMMON_EARLKW_IMPORT, COMMON_EARLKW_MODULE}

// Types
#define COMMON_EARLTY_INT32 "int"
#define COMMON_EARLTY_STR   "str"
#define COMMON_EARLTY_UNIT  "void"
#define COMMON_EARLTY_ASCPL {COMMON_EARLTY_INT32, COMMON_EARLTY_STR, COMMON_EARLTY_UNIT}

#define COMMON_EARL_COMMENT "#"

#endif // COMMON_H
