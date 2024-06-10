#include "earl.hpp"

using namespace earl::runtime;

variable::Obj::Obj(Token *id, value::Obj value) : m_id(id), m_value(value) {}

earl::primitive::Type variable::Obj::type(void) const {
    return m_value.type();
}

const std::string &variable::Obj::id(void) const {
    return m_id->lexeme();
}