#include "err.hpp"
#include "earl.hpp"

using namespace earl::value;

/*** INT ***/

Int::Int(std::optional<int> value) : m_value(value) {}

int Int::value(void) {
    if (!m_value.has_value())
        ERR(Err::Type::Fatal, "tried to unwrap None value");
    return m_value.value();
}

Type Int::type(void) const {
    return Type::Int;
}

/*** STR ***/

Str::Str(std::optional<std::string> value) : m_value(std::move(value)) {}

std::string &Str::value(void) {
    if (!m_value.has_value())
        ERR(Err::Type::Fatal, "tried to unwrap None value");
    return m_value.value();
}

Type Str::type(void) const {
    return Type::Str;
}

/*** VOID ***/

Void::Void(std::optional<void *> value) : m_value(value) {}

void *Void::value(void) {
    if (!m_value.has_value())
        ERR(Err::Type::Fatal, "tried to unwrap None value");
    return m_value.value();
}

Type Void::type(void) const {
    return Type::Void;
}

/*** LIST ***/

List::List(std::optional<std::vector<Obj>> value) : m_value(std::move(value)) {}

std::vector<Obj> &List::value(void) {
    if (!m_value.has_value())
        ERR(Err::Type::Fatal, "tried to unwrap None value");
    return m_value.value();
}

Type List::type(void) const {
    return Type::List;
}
