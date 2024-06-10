#include "earl.hpp"

using namespace EARL::Runtime;

Value::Obj::Obj(Value::Int val) : m_value(val) {}
Value::Obj::Obj(Value::Str val) : m_value(val) {}
Value::Obj::Obj(Value::Void val) : m_value(val) {}
Value::Obj::Obj(Value::List val) : m_value(val) {}

Value::RuntimeValue Value::Obj::get() const {
    return m_value;
}

Variable::Obj::Obj(Token *id, Value::Obj value) : m_id(id), m_value(value) {}

Function::Obj::Obj(Token *id,
                   Token *rettype,
                   std::vector<Variable::Obj *> args,
                   StmtBlock *block,
                   uint32_t attrs)
    : m_id(id), m_rettype(rettype), m_args(std::move(args)), m_block(block), m_attrs(attrs) {}

void Function::Obj::push_scope(void) {}

void Function::Obj::pop_scope(void) {}

void Function::Obj::new_scope_context(void) {}

void Function::Obj::drop_scope_context(void) {}

bool Function::Obj::has_local(const std::string &id) const {}

Variable::Obj *Function::Obj::get_local(const std::string &id) const {}

void Function::Obj::add_local(Variable::Obj *var) {}

size_t Function::Obj::context_size(void) const {}

void Function::Obj::remove_local(const std::string &id) {}

void Function::Obj::remove_local(const Variable::Obj &var) {}

bool is_world(void);