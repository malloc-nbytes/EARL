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

#ifndef EARLFUNC_H
#define EARLFUNC_H

#include <memory>

#include "ast.hpp"
#include "earlty.hpp"
#include "earlvar.hpp"
#include "token.hpp"
#include "scope.hpp"

/// @brief Provides the EarlFunc namespace
namespace EarlFunc {

    /// @brief The structure of a function in EARL
    struct Func {
        /// @brief The identifier of the function
        Token *m_id;

        /// @brief The return type of the function
        EarlTy::Type m_rettype;

        /// @brief The arguments that the function needs
        std::vector<EarlVar *> m_args;

        /// @brief The function body
        StmtBlock *m_block;

        /// @brief The local scope of the function
        std::vector<Scope<std::string, EarlVar *>> m_local_scope;

        Func(Token *id, EarlTy::Type rettype, std::vector<EarlVar *> args, StmtBlock *block);

        ~Func() = default;

        /// @brief Push a new local scope
        void push_scope(void);

        /// @brief Pop a local scope
        void pop_scope(void);

        // @brief Create a new context in the local scope
        void new_scope_context(void);

        // @brief Remove a scope context
        void drop_scope_context(void);

        /// @brief Check if an `EarlVar` is in the local scope
        /// @param id The identifier of the `EarlVar`
        bool contains_local_earlvar(const std::string &id);

        /// @brief Get an `EarlVar` in the local scope
        /// @param id The identifier of the `EarlVar`
        EarlVar *get_local_earlvar(const std::string &id);

        /// @brief Add an `EarlVar` to the local scope
        /// @param var The `EarlVar` to add
        void add_local_earlvar(EarlVar *var);

        /// @brief Get the number of 'contexts' (not to be confused with `Ctx` [src/include/ctx.hpp]).
        size_t context_size(void);
    };
};

#endif // EARLFUNC_H
