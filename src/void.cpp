#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

Void::Void(void *value) : m_value(value) {}

void *Void::value(void) {
    return m_value;
}

Type Void::type(void) const {
    return Type::Void;
}

Obj *Void::binop(Token *op, Obj *other) {
    (void)other;
    Err::err_wtok(op);
    ERR(Err::Type::Types, "invalid binary operation on Void types");
    return nullptr; // unreachable
}

bool Void::boolean(void) {
    return false;
}

void Void::mutate(Obj *other) {
    (void)other;
    UNIMPLEMENTED("Void::mutate");
}

Obj *Void::copy(void) {
    UNIMPLEMENTED("Void::copy");
}

