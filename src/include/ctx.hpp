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
    Ctx() = default;
    ~Ctx() = default;

    void set_function(earl::runtime::function::Obj &func);
    void unset_function(void);

    void push_scope(void);
    void pop_scope(void);

    void register_variable(earl::runtime::variable::Obj *var);
    void unregister_variable(earl::runtime::variable::Obj *var);
    void unregister_variable(const std::string &id);

    void register_function(earl::runtime::function::Obj *func);
    void unregister_function(earl::runtime::function::Obj *func);
    void unregister_function(const std::string &id);

    bool variable_is_registered(earl::runtime::variable::Obj &var);
    bool variable_is_registered(const std::string &id) const;
    bool function_is_registered(earl::runtime::function::Obj &func);
    bool function_is_registered(const std::string &id) const;
    bool in_function(void) const;

    earl::runtime::variable::Obj &get_registered_variable(const std::string &id);
    earl::runtime::function::Obj &get_registered_function(const std::string &id);

    earl::runtime::variable::Obj &get_global_registered_variable(const std::string &id);

    earl::runtime::function::Obj &get_curfunc(void);

private:
    earl::runtime::function::Obj *m_curfunc;
    Scope<std::string, earl::runtime::variable::Obj *> m_globalvars;
    Scope<std::string, earl::runtime::function::Obj *> m_globalfuncs;
};

#endif // CTX_H
