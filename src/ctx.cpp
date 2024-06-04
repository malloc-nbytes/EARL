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
    assert(false);
}

void Ctx::unset_current_earlfunc(void) {
    assert(false);
}

void Ctx::pop_scope(void) {
    m_global_earlvars.pop();
    m_global_earlfuncs.pop();
}

void Ctx::push_scope(void) {
    m_global_earlvars.push();
    m_global_earlfuncs.push();
}

void Ctx::register_earlvar(EarlVar *var) {
    std::string &id = var->m_id->lexeme();
    m_global_earlvars.add(id, var);
}

void Ctx::deregister_earlvar(EarlVar *var) {
    assert(false && "unimplemented");
}

void Ctx::register_earlfunc(EarlFunc::Func *func) {
    std::string &id = func->m_id->lexeme();
    m_global_earlfuncs.add(id, func);
}

void Ctx::deregister_earlfunc(EarlFunc::Func *func) {
    assert(false && "unimplemented");
}

bool Ctx::is_registered_earlvar(const std::string &id) {
    return m_global_earlvars.contains(id);
}

bool Ctx::is_registered_earlfunc(const std::string &id) {
    return m_global_earlfuncs.contains(id);
}

EarlVar *Ctx::get_registered_earlvar(const std::string &id) {
    EarlVar **var = nullptr;

    if (in_earlfunc()) {
        auto *func = get_cur_earlfunc();
        *var = func->get_local_earlvar(id);
    }
    else {
        var = m_global_earlvars.get(id);
    }

    if (!*var) {
        ERR_WARGS(ErrType::Fatal, "variable `%s` is not in scope", id.c_str());
    }

    return *var;
}

EarlFunc::Func *Ctx::get_registered_earlfunc(const std::string &id) {
    EarlFunc::Func **func = m_global_earlfuncs.get(id);
    if (!*func) {
        ERR_WARGS(ErrType::Fatal, "function `%s` is not in scope", id.c_str());
    }
    return *func;
}

EarlVar *Ctx::get_registered_global_earlvar(const std::string &id) {
    // Check
    assert(!in_earlfunc() || get_cur_earlfunc()->context_size() == 1);
}

EarlFunc::Func *Ctx::get_cur_earlfunc(void) {
    return m_cur_earlfunc;
}

bool Ctx::in_earlfunc(void) {
    return m_cur_earlfunc != nullptr;
}