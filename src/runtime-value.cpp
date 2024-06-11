#include "earl.hpp"
#include "utils.hpp"

using namespace earl::runtime;

value::Obj::Obj(value::Int val) : m_value(val) {}
value::Obj::Obj(value::Str val) : m_value(val) {}
value::Obj::Obj(value::Unit val) : m_value(val) {}
value::Obj::Obj(value::List val) : m_value(val) {}

value::RuntimeValue value::Obj::get() const {
    return m_value;
}

earl::primitive::Type value::Obj::type(void) const {
    return m_type;
}

bool value::Obj::is_compatable(value::Obj &other) {
    if (this->type() == earl::primitive::Type::List) {
        UNIMPLEMENTED("is_compatable: list");
    }
    return this->type() == other.type();
}