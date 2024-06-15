#include <cassert>
#include <iostream>

#include "ctx.hpp"
#include "utils.hpp"
#include "err.hpp"

Ctx::Ctx() : m_curfunc(nullptr) {
    // earl::primitive::fill_typemap();
}

void Ctx::set_function(earl::function::Obj *func) {
    m_curfunc = func;
}

earl::variable::Obj *Ctx::get_registered_variable(const std::string &id) {
    earl::variable::Obj **var = nullptr;

    if (in_function() && get_curfunc()->is_world()) {
        var = m_globalvars.get(id); // Check in global scope
        if (!var) { // Not in global, check local
            var = get_curfunc()->m_local.back().get(id);
        }
        else if (get_curfunc()->has_local(id)) { // Is in global, make sure its not in local
            ERR_WARGS(Err::Type::Redeclaration, "duplicate variable `%s`", id.c_str());
        }
    }
    else if (in_function()) {
        auto *func = get_curfunc();
        var = func->m_local.back().get(id);
    }
    else {
        var = m_globalvars.get(id);
    }

    if (!var) {
        ERR_WARGS(Err::Type::Fatal, "variable `%s` is not in scope", id.c_str());
    }

    return *var;
}

void Ctx::register_variable(earl::variable::Obj *var) {
    const std::string &id = var->id();

    if (in_function()) {
        get_curfunc()->m_local.back().add(id, var);
    }
    else {
        m_globalvars.add(id, var);
    }
}

bool Ctx::variable_is_registered(earl::variable::Obj &var) {
    UNIMPLEMENTED("variable_is_registered");
    // if (in_function()) {
    //     return get_curfunc().has_local(var.id());
    // }
    // return m_globalvars.contains(var.id());
}

bool Ctx::variable_is_registered(const std::string &id) {
    if (in_function()) {
        return get_curfunc()->has_local(id);
    }
    return m_globalvars.contains(id);
}

bool Ctx::function_is_registered(const std::string &id) {
    return m_globalfuncs.contains(id);
}

void Ctx::register_function(earl::function::Obj *func) {
    const std::string &id = func->id();
    m_globalfuncs.add(id, func);
}

earl::function::Obj *Ctx::get_registered_function(const std::string &id) {
    earl::function::Obj **func = nullptr;
    func = m_globalfuncs.get(id);
    if (!func) {
        ERR_WARGS(Err::Type::Fatal,
                  "function `%s` is not in global scope",
                  id.c_str());
    }
    return *func;
}

earl::function::Obj *Ctx::get_curfunc(void) {
    assert(m_curfunc);
    return m_curfunc;
}

bool Ctx::in_function(void) const {
    return m_curfunc != nullptr;
}
