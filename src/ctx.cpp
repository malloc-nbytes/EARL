#include <cassert>
#include <iostream>

#include "ctx.hpp"
#include "utils.hpp"
#include "err.hpp"

Ctx::Ctx() : m_curfunc(nullptr) {
    // earl::primitive::fill_typemap();
}

earl::variable::Obj *Ctx::get_registered_variable(const std::string &id) {
    earl::variable::Obj **var = nullptr;
    var = m_globalvars.get(id);
    if (!var) {
        ERR_WARGS(Err::Type::Fatal,
                  "variable `%s` is not in global scope",
                  id.c_str());
    }
    return *var;
}

void Ctx::register_variable(earl::variable::Obj *var) {
    const std::string &id = var->id();

    if (in_function()) {
        UNIMPLEMENTED("register_variable: in_function");
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
        return get_curfunc().has_local(id);
    }
    return m_globalvars.contains(id);
}

bool Ctx::function_is_registered(const std::string &id) {
    return m_globalfuncs.contains(id);
}

earl::function::Obj &Ctx::get_curfunc(void) {
    assert(m_curfunc);
    return *m_curfunc;
}

bool Ctx::in_function(void) const {
    return m_curfunc != nullptr;
}
