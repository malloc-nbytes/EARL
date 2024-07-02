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

#include <iostream>

#include "err.hpp"
#include "token.hpp"
#include "utils.hpp"

void Err::err_wtok(Token *tok) {
    std::cerr << tok->m_fp << ':' << tok->m_row << ':' << tok->m_col << '\n';
}

void Err::err_w2tok(Token *tok1, Token *tok2) {
    std::cerr << tok1->m_fp << ':' << tok1->m_row << ':' << tok1->m_col << '\n';
    std::cerr << tok2->m_fp << ':' << tok2->m_row << ':' << tok2->m_col << '\n';
}

void Err::err_wconflict(Token *tok1, Token *tok2) {
    (void)tok1;
    (void)tok2;
    UNIMPLEMENTED("Err::err_wconflict");
}

void Err::warn(std::string msg, Token *tok) {
    if (tok) {
        err_wtok(tok);
    }
    std::cerr << "[WARN] " << msg << std::endl;
}
