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
 * Provides a 'Context' struct that
 * holds all relevant information
 * that a runtime needs.
 */

#ifndef CTX_H
#define CTX_H

#include <vector>
#include <unordered_map>

#include "earl.hpp"
#include "scope.hpp"

struct Ctx {
    Ctx();
    ~Ctx() = default;

    /// @brief Set the current function being evaluated
    /// @param func The function to set
    void set_function(earl::function::Obj *func);

    /// @brief Unset the current function being evaluated
    void unset_function(void);

    /// @brief Push a new global scope
    /// @note If in a function, it will push that function's
    /// local scope instead.
    void push_scope(void);

    /// @brief Pop a global scope
    /// @note If in a function, it will pop that function's
    /// local scope instead.
    void pop_scope(void);

    /// @brief Register a variable into the current scope
    /// @note If in a function, it will add the variable
    /// to that function's local scope instead.
    /// @param var The variable to add
    void register_variable(earl::variable::Obj *var);

    /// @brief Remove a variable from the current scope
    /// @note If in a function, it will remove the variable
    /// in that function's local scope instead.
    /// @param var The identifier of the variable to remove
    void unregister_variable(const std::string &id);

    /// @brief Register a new function in the global scope
    /// @param func The function to register
    void register_function(earl::function::Obj *func);

    /// @brief Remove a function in the global scope
    /// @param func The function to remove
    void unregister_function(earl::function::Obj *func);

    /// @brief Remove a function in the global scope
    /// @param id The identifier of the function to remove
    void unregister_function(const std::string &id);

    /// @brief Check if a variable is registered in either
    /// the global or local scope of a function
    /// @param id The identifier of the variable to check
    bool variable_is_registered(const std::string &id);

    /// @brief Check if a function is registered in the global scope
    /// @param func The function to check
    bool function_is_registered(earl::function::Obj &func);

    /// @brief Check if a function is registered in the global scope
    /// @param id The identifier of the function to check
    bool function_is_registered(const std::string &id);

    /// @brief Check if the interpreter is currently in a function
    /// during runtime evaluation
    bool in_function(void) const;

    /// @brief Get a registered variable from either the global
    /// or current scope.
    /// @param id The identifier of the variable to get
    earl::variable::Obj *get_registered_variable(const std::string &id);

    /// @brief Get a registered function from the global scope
    /// @param id The identifier of the function to get
    earl::function::Obj *get_registered_function(const std::string &id);

    /// @brief Get the current function if we are in one.
    /// @note It is expected to call `in_function()` prior
    /// to calling this function
    earl::function::Obj *get_curfunc(void);

private:
    /// @brief The current function that is being
    /// evaluated during runtime
    earl::function::Obj *m_curfunc;

    /// @brief The global scope of all EARL variables
    Scope<std::string, earl::variable::Obj *> m_globalvars;

    /// @brief The global scope of all EARL functions
    Scope<std::string, earl::function::Obj *> m_globalfuncs;
};

#endif // CTX_H
