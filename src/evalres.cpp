#include "earl.hpp"

using namespace earl::runtime;

evalres::Obj::Obj(value::Obj value) : m_value(std::move(value)) {}

value::RuntimeValue evalres::Obj::value(void) {
    return m_value.get();
}

earl::primitive::Type evalres::Obj::type(void) {
    return m_value.type();
}
