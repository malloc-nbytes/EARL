#include "earl.hpp"

using namespace earl::runtime;

evalres::Obj::Obj(value::Obj value) : m_value(std::move(value)) {}

value::Obj &evalres::Obj::value(void) {
    return m_value;
}

earl::primitive::Type evalres::Obj::type(void) {
    return m_value.type();
}

std::vector<earl::primitive::Type> &evalres::Obj::non_primitive_type(void) {
    return m_type;
}