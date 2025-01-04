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

ExprPredicate::ExprPredicate(std::shared_ptr<Token> op, std::unique_ptr<Expr> rhs)
    : m_op(op), m_rhs(std::move(rhs)) {}

ExprType
ExprPredicate::get_type() const {
    return ExprType::Term;
}

ExprTermType
ExprPredicate::get_term_type() const {
    return ExprTermType::Predicate;
}

size_t
ExprPredicate::get_lineno() const {
    return m_op->m_row;
}

void
ExprPredicate::dump() const {
    token_dump_until_semi(m_op.get());
}

Token *
ExprPredicate::get_tok() const {
    return m_op.get();
}
