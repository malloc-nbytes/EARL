#include "earl.hpp"
#include "utils.hpp"

using namespace earl::runtime;

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

variable::Obj::Obj(Token *id, value::Obj value) : m_id(id), m_value(value) {}

earl::primitive::Type variable::Obj::type(void) const {
    return m_value.type();
}

function::Obj::Obj(Token *id,
                   Token *rettype,
                   std::vector<variable::Obj *> args,
                   StmtBlock *block,
                   uint32_t attrs)
    : m_id(id), m_rettype(rettype), m_args(std::move(args)), m_block(block), m_attrs(attrs) {}

void function::Obj::push_scope(void) {
    UNIMPLEMENTED("push_scope");
}

void function::Obj::pop_scope(void) {
    UNIMPLEMENTED("pop_scope");
}

void function::Obj::new_scope_context(void) {
    UNIMPLEMENTED("new_scope_context");
}

void function::Obj::drop_scope_context(void) {
    UNIMPLEMENTED("drop_scope_context");
}

bool function::Obj::has_local(const std::string &id) const {
    UNIMPLEMENTED("has_local");
}

variable::Obj *function::Obj::get_local(const std::string &id) const {
    UNIMPLEMENTED("get_local");
}

void function::Obj::add_local(variable::Obj *var) {
    UNIMPLEMENTED("add_local");
}

size_t function::Obj::context_size(void) const {
    UNIMPLEMENTED("context_size");
}

void function::Obj::remove_local(const std::string &id) {
    UNIMPLEMENTED("remove_local");
}

void function::Obj::remove_local(const variable::Obj &var) {
    UNIMPLEMENTED("remove_local");
}

bool function::Obj::is_world(void) const {
    UNIMPLEMENTED("is_world");
}