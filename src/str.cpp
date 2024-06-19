#include <cassert>

#include "earl.hpp"
#include "err.hpp"

using namespace earl::value;

Str::Str(std::string value) : m_value(std::move(value)) {}

std::string &Str::value(void) {
    return m_value;
}

void Str::fill(std::string value) {
    m_value = std::move(value);
}

Type Str::type(void) const {
    return Type::Str;
}

Obj *Str::binop(Token *op, Obj *other) {
    if (!type_is_compatable(this, other)) {
        assert(false && "cannot binop (fix this message)");
    }

    switch (op->type()) {
    case TokenType::Plus: {
        assert(other->type() == earl::value::Type::Str);
        return new Str(this->value() + dynamic_cast<Str *>(other)->value());
    } break;
    case TokenType::Double_Equals: {
        assert(other->type() == earl::value::Type::Str);
        return new Int(static_cast<int>(this->value() == dynamic_cast<Str *>(other)->value()));
    } break;
    default: {
        Err::err_wtok(op);
        ERR(Err::Type::Fatal, "invalid binary operator");
    }
    }
}

bool Str::boolean(void) {
    return true;
}

void Str::mutate(Obj *other) {
    if (!type_is_compatable(this, other)) {
        assert(false && "cannot mutate (fix this message)");
    }

    switch (other->type()) {
    case Type::Str: {
        this->fill(dynamic_cast<Str *>(other)->value());
    } break;
    default: {
        assert(false && "unreachable");
    }
    }
}

Obj *Str::copy(void) {
    return new Str(this->value());
}
