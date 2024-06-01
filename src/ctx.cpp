#include <any>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "earlty.hpp"
#include "earlvar.hpp"
#include "err.hpp"
#include "ctx.hpp"

Ctx::Ctx() {
    m_scope.emplace_back();
    m_functions.emplace_back();
}

void Ctx::pop_scope(void) {
    m_scope.pop_back();
    m_functions.pop_back();
}

void Ctx::push_scope(void) {
    m_scope.emplace_back();
    m_functions.emplace_back();
}

void Ctx::create_and_add_earlvar_to_scope(std::unique_ptr<Token> id, EarlTy::Type type, bool allocd, std::any value) {
    std::string name = id->lexeme();
    m_scope.back().emplace(name, std::make_unique<EarlVar>(std::move(id), type, allocd, std::move(value)));
}

void Ctx::add_earlvar_to_scope(std::unique_ptr<EarlVar> var) {
    std::string name = var->m_id->lexeme();
    m_scope.back().emplace(name, std::move(var));
}

bool Ctx::earlvar_in_scope(const std::string &id) {
    for (auto &map : m_scope) {
        if (map.find(id) != map.end()) {
            return true;
        }
    }
    return false;
}

EarlVar *Ctx::get_earlvar_from_scope(const std::string &id) {
    for (auto it = m_scope.rbegin(); it != m_scope.rend(); ++it) {
        auto &map = *it;
        if (map.find(id) != map.end()) {
            return map.at(id).get();
        }
    }
    ERR_WARGS(ErrType::Undeclared, "variable `%s` not found in scope", id.c_str());
    return nullptr; // unreachable
}

void Ctx::add_earlfunc_to_scope(std::unique_ptr<EarlFunc> func) {
    m_functions.back().emplace(func->m_id->lexeme(), std::move(func));
}

void Ctx::add_earlfunc_to_scope(std::unique_ptr<Token> id,
                                EarlTy::Type rettype,
                                std::vector<std::unique_ptr<EarlVar>> args,
                                std::unique_ptr<StmtBlock> block) {
    m_functions
        .back()
        .emplace(id->lexeme(),
                 std::make_unique<EarlFunc>(std::move(id),
                                            rettype,
                                            std::move(args),
                                            std::move(block)));
}

bool Ctx::earlfunc_in_scope(std::string &id) {
    for (auto &map : m_functions) {
        if (map.find(id) != map.end()) {
            return true;
        }
    }
    return false;
}

EarlFunc *Ctx::get_earlfunc_from_scope(std::string &id) {
    for (auto it = m_functions.begin(); it != m_functions.end(); ++it) {
        auto &map = *it;
        if (map.find(id) != map.end()) {
            return map.at(id).get();
        }
    }
    ERR_WARGS(ErrType::Undeclared, "function `%s` not found in scope", id.c_str());
    return nullptr; // unreachable
}

void Ctx::debug_dump_earlfuncs(void) {
    int i = 0;
    for (auto &scope : m_functions) {
        std::cout << "scope level " << i << std::endl;
        for (auto &func : scope) {
            std::cout << "  " << func.first << std::endl;
        }
        ++i;
    }
}

void Ctx::debug_dump_earlvars(void) {
}
