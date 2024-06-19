#include <cassert>

#include "earl.hpp"
#include "err.hpp"

using namespace earl::value;

Int::Int(int value) : m_value(value) {}

int Int::value(void) {
    return m_value;
}

void Int::fill(int value) {
    m_value = value;
}

Type Int::type(void) const {
    return Type::Int;
}

Obj *Int::binop(Token *op, Obj *other) {
    if (!type_is_compatable(this, other)) {
        assert(false && "cannot binop (fix this message)");
    }

    switch (op->type()) {
    case TokenType::Plus: {
        return new Int(this->value() + dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Minus: {
        return new Int(this->value() - dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Asterisk: {
        return new Int(this->value() * dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Forwardslash: {
        return new Int(this->value() / dynamic_cast<Int *>(other)->value());
    } break;
    case TokenType::Lessthan: {
        return new Int(static_cast<int>(this->value() < dynamic_cast<Int *>(other)->value()));
    }
    case TokenType::Double_Equals: {
        return new Int(static_cast<int>(this->value() == dynamic_cast<Int *>(other)->value()));
    };
    default: {
        Err::err_wtok(op);
        ERR(Err::Type::Fatal, "invalid binary operator");
    }
    }
}

bool Int::boolean(void) {
    return this->value();
}

void Int::mutate(Obj *other) {
    if (!type_is_compatable(this, other)) {
        assert(false && "cannot mutate (fix this message)");
    }

    switch (other->type()) {
    case Type::Int: {
        this->fill(dynamic_cast<Int *>(other)->value());
    } break;
    default: {
        assert(false && "unreachable");
    }
    }
}

Obj *Int::copy(void) {
    return new Int(this->value());
}
