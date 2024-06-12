#include "earl.hpp"

using namespace earl::value;

Obj(Token *id, std::unique_ptr<value::Obj> value) : m_id(id), value(std::move(value)) {}
