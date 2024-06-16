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

/**
 * This file has the goal of containing all intrinsic functions
 * and or identifiers relevant to EARL.
 */

#ifndef INTRINSICS_H
#define INTRINSICS_H

#include <vector>
#include <unordered_map>

#include "interpreter.hpp"
#include "ctx.hpp"
#include "ast.hpp"
#include "earl.hpp"

/// @brief The `Intrinsics` namespace
namespace Intrinsics {

    using IntrinsicFunction = earl::value::Obj *(*)(ExprFuncCall *, std::vector<earl::value::Obj *>&, Ctx&);

    using IntrinsicMemberFunction = earl::value::Obj *(*)(earl::value::Obj *, std::vector<earl::value::Obj *>&, Ctx&);

    /// @brief A map of all intrinsic functions in EARL
    extern const std::unordered_map<std::string, IntrinsicFunction> intrinsic_functions;

    extern const std::unordered_map<std::string, IntrinsicMemberFunction> intrinsic_member_functions;

    /// @brief Check if an identifier is the name of an intrinsic function
    /// @param id The identifier to check
    /// @return true if intrinsic, false if otherwise
    bool is_intrinsic(const std::string &id);

    bool is_member_intrinsic(const std::string &id);

    /// @brief Call an intrinsic function
    /// @note It is expected to call `is_intrinsic` before calling this function
    /// @param expr The AST node of the function call
    /// @param params value objects to pass to the function
    /// @param ctx The current global context
    earl::value::Obj *call(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx);

    earl::value::Obj *call_member(earl::value::Obj *, std::vector<earl::value::Obj *> &params, Ctx &ctx);

    /*** INTRINSIC FUNCTION IMPLEMENTATIONS ***/

    earl::value::Obj *intrinsic_member_nth(earl::value::Obj *obj, std::vector<earl::value::Obj *> &idx, Ctx &ctx);

    /// @brief Assert several expressions
    /// @param expr The AST node of the function call
    /// @param params value objects to pass to the function
    /// @param ctx The current global context
    earl::value::Obj *intrinsic_assert(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx);

    /// @brief Print several expressions
    /// @param expr The AST node of the function call
    /// @param params value objects to pass to the function
    /// @param ctx The current global context
    earl::value::Obj *intrinsic_print(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx);
};

#endif // INTRINSICS_H
