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
#include <memory>

#include "interpreter.hpp"
#include "ctx.hpp"
#include "ast.hpp"
#include "earl.hpp"

/// @brief The `Intrinsics` namespace
namespace Intrinsics {

    /// @brief All intrinsic functions must use this function signature.
    using IntrinsicFunction = std::shared_ptr<earl::value::Obj> (*)(ExprFuncCall *, std::vector<std::shared_ptr<earl::value::Obj>>&, Ctx&);

    /// @brief All intrinsic member functions must use this function signature.
    using IntrinsicMemberFunction = std::shared_ptr<earl::value::Obj> (*)(std::shared_ptr<earl::value::Obj>, std::vector<std::shared_ptr<earl::value::Obj>>&, Ctx&);

    /// @brief A map of all intrinsic functions in EARL
    extern const std::unordered_map<std::string, IntrinsicFunction> intrinsic_functions;

    /// @brief A map of all intrinsic member functions in EARL
    extern const std::unordered_map<std::string, IntrinsicMemberFunction> intrinsic_member_functions;

    /// @brief Check if an identifier is the name of an intrinsic function
    /// @param id The identifier to check
    /// @return true if intrinsic, false if otherwise
    bool is_intrinsic(const std::string &id);

    /// @brief Check if an identifier is the name of an intrinsic member function
    /// @param id The identifier to check
    /// @return true if intrinsic member, false if otherwise
    bool is_member_intrinsic(const std::string &id);

    /// @brief Call an intrinsic function
    /// @note It is expected to call `is_intrinsic` before calling this function
    /// @param expr The AST node of the function call
    /// @param params value objects to pass to the function
    /// @param ctx The current global context
    std::shared_ptr<earl::value::Obj> call(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);

    /// @brief Call an intrinsic member function
    /// @note It is expected to call `is_member_intrinsic` before calling this function
    /// @param id The identifier of the intrinsic member function
    /// @param accessor The object that the member function is a part of
    /// @param params value objects to pass to the function
    /// @note it is often expected that `params` has the size of 1 or 0
    /// @param ctx The current global context
    std::shared_ptr<earl::value::Obj> call_member(const std::string &id, std::shared_ptr<earl::value::Obj> accessor, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);

    /*** INTRINSIC FUNCTION IMPLEMENTATIONS ***/

    std::shared_ptr<earl::value::Obj> intrinsic_len(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_assert(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_println(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_print(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_open(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_type(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_unimplemented(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_exit(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_panic(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_some(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_argv(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_input(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);

    /*** INTERNAL INTRINSIC FUNCTION IMPLEMENTATIONS ***/

    std::shared_ptr<earl::value::Obj>intrinsic___internal_move__(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);
    std::shared_ptr<earl::value::Obj>intrinsic___internal_mkdir__(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);
    std::shared_ptr<earl::value::Obj>intrinsic___internal_ls__(ExprFuncCall *expr, std::vector<std::shared_ptr<earl::value::Obj>> &params, Ctx &ctx);

    /*** INTRINSIC MEMBER FUNCTION IMPLEMENTATIONS ***/

    std::shared_ptr<earl::value::Obj> intrinsic_member_nth(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &idx, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_back(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &unused, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_filter(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &closure, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_foreach(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &closure, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_rev(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &unused, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_append(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &values, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_pop(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &values, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_split(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &delim, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_remove_lines(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &unused, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_substr(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &idxs, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_dump(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &unused, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_close(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &unused, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_read(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &unused, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_write(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &param, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_writelines(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &param, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_ascii(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &unused, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_unwrap(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &unused, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_is_none(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &unused, Ctx &ctx);
    std::shared_ptr<earl::value::Obj> intrinsic_member_is_some(std::shared_ptr<earl::value::Obj> obj, std::vector<std::shared_ptr<earl::value::Obj>> &unused, Ctx &ctx);
};

#endif // INTRINSICS_H
