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

#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <stddef.h>

/// @brief All keywords in an array
extern const char *config_keywords[];

/// @brief The number of keywords that exist
extern const size_t config_keywords_len;

#define KEYWORD_LET "let"
#define KEYWORD_FN  "fn"
#define KEYWORD_RET "return"

#define KEYWORDS_ASCPL {                        \
        KEYWORD_LET,                            \
            KEYWORD_FN,                         \
            KEYWORD_RET,                        \
            }

/// @brief Check if `s` is a keyword
/// @param s The string to check
/// @return 1 if it is a keyword, or 0 if otherwise
int keywords_is_keyword(const char *s);

#endif // KEYWORDS_H
