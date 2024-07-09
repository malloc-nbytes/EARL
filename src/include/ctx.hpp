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
#include "shared-scope.hpp"

struct Ctx {
    Ctx(std::unique_ptr<Lexer> lexer, std::unique_ptr<Program> program);
    ~Ctx() = default;

    /*** Ctx ***/
    size_t stmts_len(void) const;
    Stmt *get_stmt_at(size_t i);
    Lexer *lexer(void);
    Program *program(void);
    std::string &module_();
    void set_module(const std::string &id);

    /*** Variables ***/
    bool var_exists(const std::string &id) const;
    std::shared_ptr<earl::variable::Obj> var_get(const std::string &id, bool crash_on_failure = true);
    void var_add(std::shared_ptr<earl::variable::Obj> var);

    /*** Functions ***/
    bool func_exists(const std::string &id) const;
    std::shared_ptr<earl::function::Obj> &func_get(const std::string &id, bool crash_on_failure = true);
    void func_add(std::shared_ptr<earl::function::Obj> func);

    /*** Classes ***/
    bool class_exists(const std::string &id) const;
    std::shared_ptr<earl::Class::Obj> &class_get(const std::string &id, bool crash_on_failure = true);
    void class_add(std::shared_ptr<earl::Class::Obj> klass);

    // Check if the actual statement exists
    bool class_is_defined(const std::string &id) const;
    StmtClass *class_stmt(const std::string &id);

private:
    std::unique_ptr<Lexer>   m_lexer;
    std::unique_ptr<Program> m_program;
    std::string              m_module;

    SharedScope<std::string, earl::variable::Obj> m_variables;
    SharedScope<std::string, earl::function::Obj> m_functions;
    SharedScope<std::string, earl::Class::Obj>    m_classes;

    std::vector<std::shared_ptr<Ctx>> m_children;

    std::unordered_map<std::string, StmtClass *> defined_classes;

    std::vector<std::shared_ptr<earl::value::Obj>> m_buffer;
};

#endif // CTX_H
