#include <cassert>
#include <any>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "earlty.hpp"
#include "earlvar.hpp"
#include "err.hpp"
#include "ctx.hpp"

Ctx::Ctx() {
    m_cur_earlfunc = nullptr;
}

void Ctx::set_current_earlfunc(EarlFunc::Func *func) {
    if (m_cur_earlfunc != nullptr && m_cur_earlfunc->m_id->lexeme() == func->m_id->lexeme()) {
        m_cur_earlfunc->add_new_local_scope_context();
    }
    else {
        m_cur_earlfunc = func;
    }
}

void Ctx::unset_current_earlfunc(void) {
    if (m_cur_earlfunc != nullptr && m_cur_earlfunc->m_local_scope.size() != 1) {
        m_cur_earlfunc->m_local_scope.pop_back();
    }
    else {
        m_cur_earlfunc = nullptr;
    }
}

void Ctx::push_scope_in_earlfunc(void) {
    std::string id = m_cur_earlfunc->m_id->lexeme();
    m_global_earlfuncs.get(id)->push_scope();
}

void Ctx::pop_scope_in_earlfunc(void) {
    std::string id = m_cur_earlfunc->m_id->lexeme();
    m_global_earlfuncs.get(id)->pop_scope();
}

void Ctx::pop_scope(void) {
    if (m_cur_earlfunc != nullptr) {
        pop_scope_in_earlfunc();
    }
    else {
        m_global_earlvars.pop();
        m_global_earlfuncs.pop();
    }
}

void Ctx::push_scope(void) {
    if (m_cur_earlfunc != nullptr) {
        push_scope_in_earlfunc();
    }
    else {
        m_global_earlvars.push();
        m_global_earlfuncs.push();
    }
}

EarlVar *Ctx::get_global_earlvar_from_scope(const std::string &id) {
    EarlVar *var = m_global_earlvars.get(id);
    if (!var) {
        ERR_WARGS(ErrType::Undeclared, "variable `%s` not found in scope", id.c_str());
    }
    return var;

}

void Ctx::add_earlvar_to_scope_in_earlfunc(std::unique_ptr<EarlVar> var) {
    m_cur_earlfunc->add_earlvar_to_local_scope(std::move(var));
}

void Ctx::add_earlvar_to_scope(std::unique_ptr<EarlVar> var) {
    std::string name = var->m_id->lexeme();

    // Are we currently in a function?
    if (m_cur_earlfunc != nullptr) {
        add_earlvar_to_scope_in_earlfunc(std::move(var));
    }
    else {
        // No, add to global scope.
        m_global_earlvars.add(name, std::move(var));
    }
}

bool Ctx::earlvar_in_earlfunc_scope(const std::string &id) {
    return m_cur_earlfunc->has_earlvar_in_local_scope(id);
}

bool Ctx::earlvar_in_scope(const std::string &id) {
    if (m_cur_earlfunc != nullptr) {
        return earlvar_in_earlfunc_scope(id);
    }
    return m_global_earlvars.contains(id);
}

EarlVar *Ctx::get_earlvar_in_earlfunc_scope(const std::string &id) {
    return m_cur_earlfunc->get_earlvar_from_local_scope(id);
}

EarlVar *Ctx::get_earlvar_from_scope(const std::string &id) {
    EarlVar *var = nullptr;

    if (m_cur_earlfunc != nullptr) {
        EarlVar *var = get_earlvar_in_earlfunc_scope(id);
        if (!var) {
            ERR_WARGS(ErrType::Undeclared, "variable `%s` not found in local scope", id.c_str());
        }
        return var;
    }

    var = m_global_earlvars.get(id);
    if (!var) {
        ERR_WARGS(ErrType::Undeclared, "variable `%s` not found in scope", id.c_str());
    }
    return var;
}

void Ctx::add_earlfunc_to_scope(std::unique_ptr<EarlFunc::Func> func) {
    std::string id = func->m_id->lexeme();
    m_global_earlfuncs.add(id, std::move(func));
}

bool Ctx::earlfunc_in_scope(std::string &id) {
    return m_global_earlfuncs.contains(id);
}

EarlFunc::Func *Ctx::get_earlfunc_from_scope(std::string &id) {
    EarlFunc::Func *func = m_global_earlfuncs.get(id);
    if (!func) {
        ERR_WARGS(ErrType::Undeclared, "function `%s` not found in scope", id.c_str());
    }
    return func;
}

void Ctx::debug_dump_earlfuncs(void) {
    assert(false && "unimplemented");
    // int i = 0;
    // for (auto &scope : m_functions) {
    //     std::cout << "scope level " << i << std::endl;
    //     for (auto &func : scope) {
    //         std::cout << "  " << func.first << std::endl;
    //     }
    //     ++i;
    // }
}

void Ctx::debug_dump_earlvars(void) {
    assert(false && "unimplemented");
    // int i = 0;
    // for (auto &scope : m_scope) {
    //     std::cout << "scope level " << i << std::endl;
    //     for (auto &var : scope) {
    //         std::cout << "  " << var.first << std::endl;
    //     }
    //     ++i;
    // }
}
