#include "earl.hpp"
#include "common.hpp"
#include "utils.hpp"
#include "err.hpp"

using namespace earl::runtime;

/*** EARL VALUE IMPLEMENTATION ***/

value::Obj::Obj(value::Int val) : m_value(val) {}
value::Obj::Obj(value::Str val) : m_value(val) {}
value::Obj::Obj(value::Void val) : m_value(val) {}
value::Obj::Obj(value::List val) : m_value(val) {}

value::RuntimeValue value::Obj::get() const {
    return m_value;
}

earl::primitive::Type value::Obj::type(void) const {
    return m_type;
}

/*** EARL VARIABLE IMPLEMENTATION ***/

variable::Obj::Obj(Token *id, value::Obj value) : m_id(id), m_value(value) {}

earl::primitive::Type variable::Obj::type(void) const {
    return m_value.type();
}

/*** EARL FUNCTION IMPLEMENTATION ***/

function::Obj::Obj(Token *id,
                   Token *rettype,
                   std::vector<variable::Obj *> args,
                   StmtBlock *block,
                   uint32_t attrs)
    : m_id(id), m_rettype(rettype), m_args(std::move(args)), m_block(block), m_attrs(attrs) {}

void function::Obj::push_scope(void) {
    m_local.back().push();
}

void function::Obj::pop_scope(void) {
    m_local.back().pop();
}

void function::Obj::new_scope_context(void) {
    m_local.emplace_back();
}

void function::Obj::drop_scope_context(void) {
    m_local.pop_back();
}

bool function::Obj::has_local(const std::string &id) {
    return m_local.back().contains(id);
}

variable::Obj &function::Obj::get_local(const std::string &id) {
    variable::Obj **var = m_local.back().get(id);
    if (!var) {
        ERR_WARGS(Err::Type::Fatal, "variable `%s` is not in local scope", id.c_str());
    }
    return **var;
}

void function::Obj::add_local(variable::Obj *var) {
    const std::string &id = var->m_id->lexeme();
    m_local.back().add(id, var);
}

size_t function::Obj::context_size(void) const {
    return m_local.size();
}

void function::Obj::remove_local(const std::string &id) {
    m_local.back().remove(id);
}

void function::Obj::remove_local(const variable::Obj *var) {
    const std::string &id = var->m_id->lexeme();
    m_local.back().remove(id);
}

bool function::Obj::is_world(void) const {
    return (m_attrs & static_cast<uint32_t>(FuncAttr::World)) != 0;
}

/*** EVAL RESULT IMPLEMENTATION ***/

evalres::Obj::Obj(value::Obj value) : m_value(std::move(value)) {}

value::RuntimeValue evalres::Obj::value(void) {
    return m_value.get();
}

earl::primitive::Type evalres::Obj::type(void) {
    return m_value.type();
}
