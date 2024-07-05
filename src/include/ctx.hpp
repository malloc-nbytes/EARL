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
    Ctx(std::unique_ptr<Lexer> lexer, std::unique_ptr<Program> program);
    ~Ctx() = default;

    /// @brief Set the current function being evaluated
    /// @param func The function to set
    void set_function(earl::function::Obj *func);

    /// @brief Unset the current function being evaluated
    void unset_function(void);

    void set_closure(earl::value::Closure *closure);
    void unset_closure();

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

    /// @brief Get the current module of the context
    /// @returns A token representing the module name
    Token *get_module(void);

    /// @brief Set the module name
    /// @param id The module to set
    void set_module(std::unique_ptr<Token> id);

    /// @brief Push on a new child context
    /// @param child The child context to push
    void push_child_context(std::unique_ptr<Ctx> child);

    /// @brief Get a already registered and parsed module
    /// @param id The identifier of the module
    earl::value::Module *get_registered_module(const std::string &id);

    /// @brief Retrieve a specific statement in the parsed code
    /// @param i The index of the statement
    Stmt *get_stmt(size_t i);

    /// @brief Get the total number of statements parsed
    size_t stmts_len(void) const;

    earl::value::Class *get_registered_class(const std::string &id);
    bool class_is_registered(const std::string &id);
    void register_class(earl::value::Class *klass);
    bool owns_class(const std::string id);

    void add_to_tmp_scope(earl::variable::Obj *var);
    earl::variable::Obj *get_var_from_tmp_scope(const std::string &id);
    bool var_in_tmp_scope(const std::string &id);
    void clear_tmp_scope(void);

    void debug_dump(void);

    std::vector<std::pair<StmtClass *, Ctx *>> available_classes;

    Ctx *m_parent;
    earl::value::Class *curclass;
    earl::value::Closure *curclosure;

    std::vector<earl::value::Class *> class_chain;

    /// @brief All children contexts from parsed imported files
    std::vector<std::unique_ptr<Ctx>> m_children_contexts;

    std::vector<std::unordered_map<std::string, earl::variable::Obj *>> m_tmp_scope;

    Ctx *prev;

    /// @brief The lexer associated with this module. This
    /// is needed because of ownership rules.
    std::unique_ptr<Lexer> m_lexer;

    std::vector<earl::value::Closure *> closures;

private:
    /// @brief The global scope of all EARL variables
    Scope<std::string, earl::variable::Obj *> m_globalvars;

    /// @brief The global scope of all EARL functions
    Scope<std::string, earl::function::Obj *> m_globalfuncs;

    std::unordered_map<std::string, earl::value::Class *> m_globalclasses;

    /// @brief The name of the module as a token
    std::unique_ptr<Token> m_module;

    /// @brief The parsed program produced from the `Parser`.
    /// This is needed because of ownership rules.
    std::unique_ptr<Program> m_program;

    /// @brief The stacktrace of EARL function calls.
    std::vector<earl::function::Obj *> m_stacktrace;
};

#endif // CTX_H
