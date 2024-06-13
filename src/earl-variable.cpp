#include "earl.hpp"

using namespace earl::variable;

Obj::Obj(Token *id, std::unique_ptr<earl::value::Obj> value)
    : m_id(id), m_value(std::move(value)) {}

const std::string &Obj::id(void) const {
    return m_id->lexeme();
}

earl::value::Obj *Obj::value(void) const {
    return m_value.get();
}

