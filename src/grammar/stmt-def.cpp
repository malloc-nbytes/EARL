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

StmtDef::StmtDef(std::shared_ptr<Token> id,
                 std::vector<std::pair<std::pair<std::shared_ptr<Token>, std::optional<std::shared_ptr<__Type>>>, uint32_t>> args,
                 std::optional<std::shared_ptr<__Type>> ty,
                 std::unique_ptr<StmtBlock> block,
                 uint32_t attrs,
                 std::vector<std::string> info) :
    m_id(id),
    m_args(args),
    m_ty(ty),
    m_block(std::move(block)),
    m_attrs(attrs),
    m_info(std::move(info)) {}

StmtType
StmtDef::stmt_type() const {
    return StmtType::Def;
}

size_t
StmtDef::get_lineno() const {
    assert(false);
}
