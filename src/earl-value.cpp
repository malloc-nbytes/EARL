#include "earl.hpp"

using namespace earl::value;

/*** INT ***/

Int::Int(int value) : m_value(value) {}

int Int::value(void) {
    return m_value;
}

Type Int::type(void) const {
    return Type::Int;
}

/*** STR ***/

Str::Str(std::string value) : m_value(std::move(value)) {}

std::string &Str::value(void) {
    return m_value;
}

Type Str::type(void) const {
    return Type::Str;
}

/*** VOID ***/

Void::Void(void *value) : m_value(value) {}

void *Void::value(void) {
    return m_value;
}

Type Void::type(void) const {
    return Type::Void;
}

/*** LIST ***/

List::List(std::vector<Obj> value) : m_value(std::move(value)) {}

std::vector<Obj> &List::value(void) {
    return m_value;
}

Type List::type(void) const {
    return Type::List;
}
