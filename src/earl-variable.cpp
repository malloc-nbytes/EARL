#include "earl.hpp"

using namespace earl::variable;

Obj::Obj(Token *id, std::unique_ptr<earl::value::Obj> value)
    : m_id(id), m_value(std::move(value)) {}
