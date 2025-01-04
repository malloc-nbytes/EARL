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

#include <cassert>
#include <memory>

#include "ast.hpp"

StmtLet::StmtLet(std::vector<std::shared_ptr<Token>> ids,
                 std::vector<std::shared_ptr<__Type>> tys,
                 std::unique_ptr<Expr> expr,
                 uint32_t attrs,
                 std::vector<std::string> info,
                 std::shared_ptr<Token> tok)
    : m_ids(ids),
      m_tys(tys),
      m_expr(std::move(expr)),
      m_attrs(attrs),
      m_info(std::move(info)),
      m_tok(std::move(tok)) {}

StmtType
StmtLet::stmt_type() const {
    return StmtType::Let;
}

size_t
StmtLet::get_lineno() const {
    return m_tok->m_col;
}
