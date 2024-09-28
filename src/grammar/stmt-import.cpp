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
#include "common.hpp"

StmtImport::StmtImport(std::shared_ptr<Token> fp,
                       std::optional<std::shared_ptr<Token>> depth,
                       std::optional<std::shared_ptr<Token>> as)
    : m_fp(fp), m_depth(depth), m_as(as) {
    // __m_depth = m_depth->lexeme() == COMMON_EARLKW_ALMOST ? COMMON_DEPTH_ALMOST : COMMON_DEPTH_FULL;
    if (!m_depth.has_value())
        __m_depth = COMMON_DEPTH_FULL;
    else if (m_depth.value()->lexeme() == COMMON_EARLKW_ALMOST)
        __m_depth = COMMON_DEPTH_ALMOST;
    else
        __m_depth = COMMON_DEPTH_FULL;
}

StmtType
StmtImport::stmt_type() const {
    return StmtType::Import;
}
