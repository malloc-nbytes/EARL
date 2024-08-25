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

#include "ast.hpp"
#include "lexer.hpp"
#include "earl.hpp"
#include "shared-scope.hpp"

enum class CtxType {
    World,
    Function,
    Class,
    Closure,
};

struct WorldCtx;

struct Ctx {
    virtual ~Ctx() = default;

    virtual CtxType type(void) const = 0;
    virtual void push_scope(void) = 0;
    virtual void pop_scope(void) = 0;
    virtual void variable_add(std::shared_ptr<earl::variable::Obj> var) = 0;
    virtual bool variable_exists(const std::string &id) = 0;
    virtual std::shared_ptr<earl::variable::Obj> variable_get(const std::string &id) = 0;
    virtual void variable_remove(const std::string &id) = 0;
    virtual void function_add(std::shared_ptr<earl::function::Obj> func) = 0;
    virtual bool function_exists(const std::string &id) = 0;
    virtual std::shared_ptr<earl::function::Obj> function_get(const std::string &id) = 0;
    virtual bool closure_exists(const std::string &id) = 0;
    virtual WorldCtx *get_world(void) = 0;

    SharedScope<std::string, earl::variable::Obj> m_scope;
    SharedScope<std::string, earl::function::Obj> m_funcs;
};

struct WorldCtx : public Ctx {
    WorldCtx(std::unique_ptr<Lexer> lexer, std::unique_ptr<Program> program);
    WorldCtx();
    ~WorldCtx() = default;

    size_t stmts_len(void) const;
    Stmt *stmt_at(size_t idx);
    void set_mod(std::string id);
    const std::string &get_mod(void) const;
    void add_import(std::shared_ptr<Ctx> ctx);
    std::shared_ptr<Ctx> *get_import(const std::string &id);
    void define_class(StmtClass *klass);
    bool class_is_defined(const std::string &id) const;
    StmtClass *class_get(const std::string &id);
    void debug_dump_defined_classes(void) const;
    void debug_dump_variables(void) const;
    bool enum_exists(const std::string &id) const;
    std::shared_ptr<earl::value::Enum> enum_get(const std::string &id);
    void strip_funs_and_classes(void);

    CtxType type(void) const override;
    void push_scope(void) override;
    void pop_scope(void) override;
    void variable_add(std::shared_ptr<earl::variable::Obj> var) override;
    bool variable_exists(const std::string &id) override;
    std::shared_ptr<earl::variable::Obj> variable_get(const std::string &id) override;
    void variable_remove(const std::string &id) override;
    void function_add(std::shared_ptr<earl::function::Obj> func) override;
    bool function_exists(const std::string &id) override;
    std::shared_ptr<earl::function::Obj> function_get(const std::string &id) override;
    bool closure_exists(const std::string &id) override;
    void enum_add(std::shared_ptr<earl::value::Enum> _enum);
    WorldCtx *get_world(void) override;

    // REPL
    void add_repl_lexer(std::unique_ptr<Lexer> lexer);
    void add_repl_program(std::unique_ptr<Program> program);

private:
    std::string m_mod;
    std::vector<std::shared_ptr<Ctx>> m_imports;
    std::unique_ptr<Lexer> m_lexer;
    std::unique_ptr<Program> m_program;
    std::unordered_map<std::string, StmtClass *> m_defined_classes;
    std::unordered_map<std::string, std::shared_ptr<earl::value::Enum>> m_enums;

    // REPL
    std::vector<std::unique_ptr<Lexer>> m_repl_lexers;
    std::vector<std::unique_ptr<Program>> m_repl_programs;
};

struct FunctionCtx : public Ctx {
    FunctionCtx(std::shared_ptr<Ctx> owner, uint32_t attrs);
    ~FunctionCtx() = default;

    bool in_class(void) const;
    std::shared_ptr<Ctx> &get_outer_class_owner_ctx(void);
    std::shared_ptr<Ctx> &get_owner(void);
    std::shared_ptr<Ctx> &get_outer_world_owner(void);
    void debug_dump_variables(void) const;

    CtxType type(void) const override;
    void push_scope(void) override;
    void pop_scope(void) override;
    void variable_add(std::shared_ptr<earl::variable::Obj> var) override;
    bool variable_exists(const std::string &id) override;
    std::shared_ptr<earl::variable::Obj> variable_get(const std::string &id) override;
    void variable_remove(const std::string &id) override;
    void function_add(std::shared_ptr<earl::function::Obj> func) override;
    bool function_exists(const std::string &id) override;
    std::shared_ptr<earl::function::Obj> function_get(const std::string &id) override;
    bool closure_exists(const std::string &id) override;
    WorldCtx *get_world(void) override;

    void setrec(void);
    void set_curfunc(const std::string &id);
    const std::string &get_curfuncid(void);

private:
    std::shared_ptr<Ctx> m_owner; // The MAIN owner
    std::shared_ptr<Ctx> m_immediate_owner;
    uint32_t m_attrs;
    bool m_in_rec;
    std::string m_curfunc_id;
};

struct ClassCtx : public Ctx {
    ClassCtx(std::shared_ptr<Ctx> owner);
    ClassCtx(std::shared_ptr<Ctx> owner, SharedScope<std::string, earl::variable::Obj> scope);
    ClassCtx(std::shared_ptr<Ctx> owner, SharedScope<std::string, earl::variable::Obj> scope, SharedScope<std::string, earl::function::Obj> funcs);
    ~ClassCtx() = default;

    std::shared_ptr<Ctx> &get_owner(void);
    void function_debug_dump(void) const;
    void fill___m_class_constructor_tmp_args(std::shared_ptr<earl::variable::Obj> &var);
    void clear___m_class_constructor_tmp_args(void);
    std::unordered_map<std::string, std::shared_ptr<earl::variable::Obj>> &
    get___m_class_constructor_tmp_args(void);
    bool variable_exists_wo__m_class_constructor_tmp_args(const std::string &id);
    std::shared_ptr<ClassCtx> deep_copy(void);
    std::shared_ptr<ClassCtx> shallow_copy(void);
    std::vector<std::shared_ptr<earl::variable::Obj>> get_printable_members(void);

    CtxType type(void) const override;
    void push_scope(void) override;
    void pop_scope(void) override;
    void variable_add(std::shared_ptr<earl::variable::Obj> var) override;
    bool variable_exists(const std::string &id) override;
    std::shared_ptr<earl::variable::Obj> variable_get(const std::string &id) override;
    void variable_remove(const std::string &id) override;
    void function_add(std::shared_ptr<earl::function::Obj> func) override;
    bool function_exists(const std::string &id) override;
    std::shared_ptr<earl::function::Obj> function_get(const std::string &id) override;
    bool closure_exists(const std::string &id) override;
    WorldCtx *get_world(void) override;
    std::shared_ptr<Ctx> &get_world_owner(void);

private:
    std::shared_ptr<Ctx> m_owner;

    // Used in the [x, y, ..., N] arguments when creating a new class.
    // This should only be available for the duration of eval_class_instantiation()
    // for the class members as well as providing visibility to the constructor().
    // Then it should be cleared.
    std::unordered_map<std::string, std::shared_ptr<earl::variable::Obj>> __m_class_constructor_tmp_args;
};

struct ClosureCtx : public Ctx {
    ClosureCtx(std::shared_ptr<Ctx> owner);
    ~ClosureCtx() = default;

    std::shared_ptr<Ctx> &get_owner(void);
    std::shared_ptr<Ctx> &get_outer_world_owner(void);
    void assert_variable_does_not_exist_for_recursive_cl(const std::string &id) const;
    bool in_class(void) const;
    std::shared_ptr<Ctx> &get_outer_class_owner_ctx(void);

    CtxType type(void) const override;
    void push_scope(void) override;
    void pop_scope(void) override;
    void variable_add(std::shared_ptr<earl::variable::Obj> var) override;
    bool variable_exists(const std::string &id) override;
    std::shared_ptr<earl::variable::Obj> variable_get(const std::string &id) override;
    void variable_remove(const std::string &id) override;
    void function_add(std::shared_ptr<earl::function::Obj> func) override;
    bool function_exists(const std::string &id) override;
    std::shared_ptr<earl::function::Obj> function_get(const std::string &id) override;
    bool closure_exists(const std::string &id) override;
    WorldCtx *get_world(void) override;

private:
    std::shared_ptr<Ctx> m_owner;
};

#endif // CTX_H
