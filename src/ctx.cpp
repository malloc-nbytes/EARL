#include <cassert>
#include <iostream>

#include "err.hpp"
#include "ctx.hpp"

Ctx::Ctx() : m_curfunc(nullptr) {
    // earl::primitive::fill_typemap();
}

bool Ctx::variable_is_registered(earl::variable::Obj &var) {
    if (in_function()) {
        return get_curfunc().has_local(var.id());
    }
    return m_globalvars.contains(var.id());
}

bool Ctx::variable_is_registered(const std::string &id) {
    // if (in_function()) {
    //     return get_curfunc().has_local(id);
    // }
    // return m_globalvars.contains(id);
}

earl::function::Obj &Ctx::get_curfunc(void) {
    // assert(m_curfunc);
    // return *m_curfunc;
}

bool Ctx::in_function(void) const {
    // return m_curfunc != nullptr;
}
