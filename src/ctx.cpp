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
    // m_scope.emplace_back();
    // m_functions.emplace_back();
}

void Ctx::pop_scope(void) {
    // m_scope.pop_back(); // rem
    // m_functions.pop_back(); // rem

    m_global_earlvars.pop();
    m_global_earlfuncs.pop();
}

void Ctx::push_scope(void) {
    // m_scope.emplace_back(); // rem
    // m_functions.emplace_back(); // rem

    m_global_earlvars.push();
    m_global_earlfuncs.push();
}

void Ctx::add_earlvar_to_scope(std::unique_ptr<EarlVar> var) {
    std::string name = var->m_id->lexeme();
    m_scope.back().emplace(name, std::move(var)); // rem

    m_global_earlvars.add(name, std::move(var));
}

bool Ctx::earlvar_in_scope(const std::string &id) {
    return m_global_earlvars.contains(id);

    // for (auto &map : m_scope) {
    //     if (map.find(id) != map.end()) {
    //         return true;
    //     }
    // }
    // return false;
}

EarlVar *Ctx::get_earlvar_from_scope(const std::string &id) {
    EarlVar *var = m_global_earlvars.get(id);
    if (!var) {
        ERR_WARGS(ErrType::Undeclared, "variable `%s` not found in scope", id.c_str());
    }
    return var;

    // for (auto it = m_scope.rbegin(); it != m_scope.rend(); ++it) {
    //     auto &map = *it;
    //     if (map.find(id) != map.end()) {
    //         return map.at(id).get();
    //     }
    // }
    // ERR_WARGS(ErrType::Undeclared, "variable `%s` not found in scope", id.c_str());
    // return nullptr; // unreachable
}

void Ctx::add_earlfunc_to_scope(std::unique_ptr<EarlFunc::Func> func) {
    std::string id = func->m_id->lexeme();
    m_global_earlfuncs.add(id, std::move(func));
    // m_functions.back().emplace(func->m_id->lexeme(), std::move(func));
}

bool Ctx::earlfunc_in_scope(std::string &id) {
    return m_global_earlfuncs.contains(id);

    // for (auto &map : m_functions) {
    //     if (map.find(id) != map.end()) {
    //         return true;
    //     }
    // }
    // return false;
}

EarlFunc::Func *Ctx::get_earlfunc_from_scope(std::string &id) {
    EarlFunc::Func *func = m_global_earlfuncs.get(id);

    if (!func) {
        ERR_WARGS(ErrType::Undeclared, "function `%s` not found in scope", id.c_str());
    }

    return func;

    // for (auto it = m_functions.begin(); it != m_functions.end(); ++it) {
    //     auto &map = *it;
    //     if (map.find(id) != map.end()) {
    //         return map.at(id).get();
    //     }
    // }
    // ERR_WARGS(ErrType::Undeclared, "function `%s` not found in scope", id.c_str());
    // return nullptr; // unreachable
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
