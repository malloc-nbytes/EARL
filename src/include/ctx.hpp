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
 * Provides a 'Context' struct that
 * holds all relevant information
 * that a runtime needs.
 */

#ifndef CTX_H
#define CTX_H

#include <any>
#include <vector>
#include <unordered_map>

#include "earlty.hpp"
#include "earlvar.hpp"
#include "earlfunc.hpp"
#include "scope.hpp"

/// @brief The global "Context" of the runtime interpreter
class Ctx {
    EarlFunc::Func *m_cur_earlfunc;

    Scope<std::string, EarlVar *> m_global_earlvars;
    Scope<std::string, EarlFunc::Func *> m_global_earlfuncs;

public:
    Ctx();
    ~Ctx() = default;

    /// @brief Sets the current running function during runtime
    /// @param func The function to set
    void set_current_earlfunc(EarlFunc::Func *func);

    /// @brief Unsets the current function during runtime
    void unset_current_earlfunc(void);


    /// @brief Pops the current scope from the context
    void pop_scope(void);

    /// @brief Pushes a new scope in the context
    void push_scope(void);

    /// @brief Register an EarlVar into the current scope
    /// @param var The variable to add
    void register_earlvar(EarlVar *var);

    /// @brief Deregister an EarlVar in the current scope.
    /// @param var The variable to remove
    void deregister_earlvar(EarlVar *var);

    // Add an EarlFunc to the current scope.
    void register_earlfunc(EarlFunc::Func *func);
    // Remove an EarlFunc to the current scope.
    void deregister_earlfunc(EarlFunc::Func *func);

    // Check if an EarlVar with the name of `id` is registered.
    bool is_registered_earlvar(const std::string &id);
    // Check if an EarlFunc with the name of `id` is registered.
    bool is_registered_earlfunc(const std::string &id);

    // Retrive a registered EarlVar from the current scope.
    EarlVar *get_registered_earlvar(const std::string &id);
    // Retrive a registered EarlFunc from the current scope.
    EarlFunc::Func *get_registered_earlfunc(const std::string &id);

    // Get the EarlVar with the id of `id` ONLY in the global scope.
    EarlVar *get_registered_global_earlvar(const std::string &id);

    // Get the EarlFunc that we are currently in. Can be null.
    EarlFunc::Func *get_cur_earlfunc(void);

    // Check if we are currently in an EarlFunc.
    bool in_earlfunc(void);
};

#endif // CTX_H
