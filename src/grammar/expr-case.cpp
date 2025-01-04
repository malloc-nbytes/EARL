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

ExprCase::ExprCase(std::unique_ptr<Expr> expr, std::vector<std::unique_ptr<Case>> cases)
    : m_expr(std::move(expr)), m_cases(std::move(cases)) {}

ExprCase::Case::Case(std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
    : m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}

ExprType
ExprCase::get_type() const {
    return ExprType::Term;
}

ExprTermType
ExprCase::get_term_type() const {
    return ExprTermType::Case;
}

size_t
ExprCase::get_lineno() const {
    assert(false);
}
