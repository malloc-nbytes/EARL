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

#ifndef EARLVAR_H
#define EARLVAR_H

#include <any>
#include <memory>

#include "token.hpp"
#include "earlty.hpp"

/// @brief An `EarlVar` is the value of all variables
/// that are created during runtime.
struct EarlVar {
    /// @brief The unique identifier'
    Token *m_id;

    /// @brief The type of the variable in
    /// terms of an `EARL Type`
    EarlTy::Type m_type;

    /// @brief How many nested lists (if any).
    size_t m_list_depth;

    /// @brief Whether it is heap or
    /// stack alloc'd
    bool m_allocd;

    /// @brief The actual value that the variable holds
    /// during runtime.
    std::any m_value;

    /// @brief The number of references that the
    /// variable currently has. Namely the
    /// number of `owners` that is has.
    uint32_t m_refcount;

    EarlVar(Token *id, EarlTy::Type type, size_t list_depth, bool allocd,
            std::any value = nullptr, uint32_t refcount = 1);

    ~EarlVar() = default;

    void set_value(std::any value);
};

#endif // EARLVAR_H
