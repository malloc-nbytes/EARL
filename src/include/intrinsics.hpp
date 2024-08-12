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

#define __INTR_ARGS_MUSTBE_SIZE(args, sz, fn)                           \
    do {                                                                \
        if (args.size() != sz) {                                        \
            std::string __Msg = "function `" fn "` expects "+std::to_string(sz)+" arguments but "+std::to_string(args.size())+" were supplied"; \
            throw InterpreterException(__Msg);                          \
        }                                                               \
    } while (0)

#define __MEMBER_INTR_ARGS_MUSTNOT_BE_0(args, fn)                                                      \
    do {                                                                                               \
        if (args.size() == 0) {                                         \
            std::string __Msg = "member intrinsic `" fn "` expects grater than 0 arguments"; \
            throw InterpreterException(__Msg);                          \
        }                                                               \
    } while (0)

#define __MEMBER_INTR_ARGS_MUSTBE_GTSIZE(args, sz, fn)                                                                       \
    do {                                                                                                                     \
        if (args.size() < sz) {                                         \
            std::string __Msg = "member intrinsic `" fn "` expects greater than "+std::to_string(sz)+" arguments but " \
                +std::to_string(args.size())+" were supplied";          \
            throw InterpreterException(__Msg);                          \
        }                                                               \
    } while (0)

#define __INTR_ARG_MUSTBE_TYPE_COMPAT(arg, ty, loc, fn)                                                        \
    do {                                                                                                              \
        if (!earl::value::type_is_compatable(arg->type(), ty)) {        \
            std::string __Msg = "the "+std::to_string(loc)+"argument of function `" fn "` expects type `" \
                +earl::value::type_to_str(ty)+"` but got `"+earl::value::type_to_str(arg->type())+"`"; \
            throw InterpreterException(__Msg);                          \
        }                                                               \
    } while (0)

#define __MEMBER_INTR_ARG_MUSTBE_TYPE_COMPAT_OR(arg, ty1, ty2, loc, fn)                                               \
    do {                                                                                                              \
        if (!earl::value::type_is_compatable(arg->type(), ty1) && !earl::value::type_is_compatable(arg->type(), ty2)) { \
            std::string __Msg = "the "+std::to_string(loc)+" argument of function `" fn "` expects either type `" \
                +earl::value::type_to_str(ty1)+"` or `"+earl::value::type_to_str(ty2)+"` but got `"+earl::value::type_to_str(arg->type())+"`"; \
            throw InterpreterException(__Msg);                          \
        } \
    } while (0)

#define __MEMBER_INTR_ARG_MUSTBE_TYPE_COMPAT_OR_LST(arg, tys, loc, fn) \
    do { \
        bool __ok = false;     \
        for (auto &ty : tys) { \
            if (earl::value::type_is_compatable(arg->type(), ty)) \
                __ok = true; \
        } \
        if (!__ok) {                                                    \
            std::string __Msg = "the "+std::to_string(loc)+" argument of function `" fn "` expects type-adjacent `" \
                +earl::value::type_to_str(tys.at(0))+"` but got `"+earl::value::type_to_str(arg->type())+"`"; \
        }                                                               \
    } while (0)

/// @brief The `Intrinsics` namespace
namespace Intrinsics {

    /// @brief All intrinsic functions must use this function signature.
    using IntrinsicFunction =
        std::shared_ptr<earl::value::Obj> (*)(std::vector<std::shared_ptr<earl::value::Obj>>&,
                                              std::shared_ptr<Ctx>&);

    /// @brief All intrinsic member functions must use this function signature.
    using IntrinsicMemberFunction =
        std::shared_ptr<earl::value::Obj> (*)(std::shared_ptr<earl::value::Obj>,
                                              std::vector<std::shared_ptr<earl::value::Obj>>&,
                                              std::shared_ptr<Ctx>&);

    /// @brief A map of all intrinsic functions in EARL
    extern const std::unordered_map<std::string, IntrinsicFunction> intrinsic_functions;

    /// @brief A map of all intrinsic member functions in EARL
    extern const std::unordered_map<std::string, IntrinsicMemberFunction> intrinsic_member_functions;
    extern const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction> intrinsic_list_member_functions;
    extern const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction> intrinsic_str_member_functions;
    extern const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction> intrinsic_char_member_functions;
    extern const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction> intrinsic_option_member_functions;
    extern const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction> intrinsic_file_member_functions;
    extern const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction> intrinsic_tuple_member_functions;

    /// @brief Check if an identifier is the name of an intrinsic function
    /// @param id The identifier to check
    /// @return true if intrinsic, false if otherwise
    bool is_intrinsic(const std::string &id);

    /// @brief Check if an identifier is the name of an intrinsic member function
    /// @param id The identifier to check
    /// @return true if intrinsic member, false if otherwise
    bool is_member_intrinsic(const std::string &id, int ty = -1);

    /// @brief Call an intrinsic function
    /// @note It is expected to call `is_intrinsic` before calling this function
    /// @param expr The AST node of the function call
    /// @param params value objects to pass to the function
    /// @param ctx The current global context
    std::shared_ptr<earl::value::Obj> call(const std::string &id,
                                           std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                           std::shared_ptr<Ctx> &ctx);

    /// @brief Call an intrinsic member function
    /// @note It is expected to call `is_member_intrinsic` before calling this function
    /// @param id The identifier of the intrinsic member function
    /// @param accessor The object that the member function is a part of
    /// @param params value objects to pass to the function
    /// @note it is often expected that `params` has the size of 1 or 0
    /// @param ctx The current global context
    std::shared_ptr<earl::value::Obj> call_member(const std::string &id,
                                                  earl::value::Type type,
                                                  std::shared_ptr<earl::value::Obj> accessor,
                                                  std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                                  std::shared_ptr<Ctx> &ctx);

    /*** INTRINSIC FUNCTION IMPLEMENTATIONS ***/

    std::shared_ptr<earl::value::Obj>
    intrinsic_len(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                  std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_tuple(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                    std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_list(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                    std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_bool(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                   std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_str(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                  std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_int(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                  std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_float(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                    std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_assert(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                     std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_println(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                      std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_fprintln(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                      std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_print(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                    std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_fprint(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                     std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_open(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                   std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_type(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                   std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_unimplemented(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                            std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_exit(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                   std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_panic(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                    std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_some(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                   std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_argv(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                   std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_input(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                    std::shared_ptr<Ctx> &ctx);

    /*** INTERNAL INTRINSIC FUNCTION IMPLEMENTATIONS ***/

    std::shared_ptr<earl::value::Obj>
    intrinsic___internal_move__(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic___internal_mkdir__(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                                 std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic___internal_ls__(std::vector<std::shared_ptr<earl::value::Obj>> &params,
                              std::shared_ptr<Ctx> &ctx);

    /*** INTRINSIC MEMBER FUNCTION IMPLEMENTATIONS ***/

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_nth(std::shared_ptr<earl::value::Obj> obj,
                         std::vector<std::shared_ptr<earl::value::Obj>> &idx,
                         std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_back(std::shared_ptr<earl::value::Obj> obj,
                          std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                          std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_filter(std::shared_ptr<earl::value::Obj> obj,
                            std::vector<std::shared_ptr<earl::value::Obj>> &closure,
                            std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_foreach(std::shared_ptr<earl::value::Obj> obj,
                             std::vector<std::shared_ptr<earl::value::Obj>> &closure,
                             std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_rev(std::shared_ptr<earl::value::Obj> obj,
                         std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                         std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_append(std::shared_ptr<earl::value::Obj> obj,
                            std::vector<std::shared_ptr<earl::value::Obj>> &values,
                            std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_pop(std::shared_ptr<earl::value::Obj> obj,
                         std::vector<std::shared_ptr<earl::value::Obj>> &values,
                         std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_split(std::shared_ptr<earl::value::Obj> obj,
                           std::vector<std::shared_ptr<earl::value::Obj>> &delim,
                           std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_trim(std::shared_ptr<earl::value::Obj> obj,
                          std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                          std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_remove_lines(std::shared_ptr<earl::value::Obj> obj,
                                  std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                                  std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_substr(std::shared_ptr<earl::value::Obj> obj,
                            std::vector<std::shared_ptr<earl::value::Obj>> &idxs,
                            std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_dump(std::shared_ptr<earl::value::Obj> obj,
                          std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                          std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_close(std::shared_ptr<earl::value::Obj> obj,
                           std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                           std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_read(std::shared_ptr<earl::value::Obj> obj,
                          std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                          std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_write(std::shared_ptr<earl::value::Obj> obj,
                           std::vector<std::shared_ptr<earl::value::Obj>> &param,
                           std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_writelines(std::shared_ptr<earl::value::Obj> obj,
                                std::vector<std::shared_ptr<earl::value::Obj>> &param,
                                std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_ascii(std::shared_ptr<earl::value::Obj> obj,
                           std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                           std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_unwrap(std::shared_ptr<earl::value::Obj> obj,
                            std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                            std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_is_none(std::shared_ptr<earl::value::Obj> obj,
                             std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                             std::shared_ptr<Ctx> &ctx);

    std::shared_ptr<earl::value::Obj>
    intrinsic_member_is_some(std::shared_ptr<earl::value::Obj> obj,
                             std::vector<std::shared_ptr<earl::value::Obj>> &unused,
                             std::shared_ptr<Ctx> &ctx);
};

#endif // INTRINSICS_H
