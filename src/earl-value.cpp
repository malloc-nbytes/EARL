#include "earl.hpp"
#include "token.hpp"
#include "common.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

/*** INT ***/

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

/*** STR ***/

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
        return new Str(this->value() + dynamic_cast<Str *>(other)->value());
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

/*** VOID ***/

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

/*** LIST ***/

List::List(std::vector<Obj *> value) : m_value(std::move(value)) {}

void List::fill(std::vector<Obj *> value) {
    m_value = std::move(value);
}

std::vector<Obj *> &List::value(void) {
    return m_value;
}

Type List::type(void) const {
    return Type::List;
}

Obj *List::nth(Obj *idx) {
    switch (idx->type()) {
    case Type::Int: return this->value()[dynamic_cast<Int *>(idx)->value()];
    default: assert(false && "ur stupid");
    }
}

Obj *List::binop(Token *op, Obj *other) {
    if (!type_is_compatable(this, other)) {
        assert(false && "cannot binop (fix this message)");
    }

    switch (op->type()) {
    case TokenType::Plus: {
        auto list = new List(this->value());
        list->value().insert(list->value().end(), dynamic_cast<List *>(other)->value().begin(), dynamic_cast<List *>(other)->value().end());
        return list;
    } break;
    default: {
        Err::err_wtok(op);
        ERR(Err::Type::Fatal, "invalid binary operator");
    }
    }

    return nullptr; // unreachable
}

bool List::boolean(void) {
    return true;
}

void List::mutate(Obj *other) {
    (void)other;
    UNIMPLEMENTED("List::mutate");
}

Obj *List::copy(void) {
    auto list = new List(this->value());
    return list;
}

/*** OTHER ***/

static const std::unordered_map<std::string, earl::value::Type> str_to_type_map = {
    {COMMON_EARLTY_INT32, earl::value::Type::Int},
    {COMMON_EARLTY_STR, earl::value::Type::Str},
    {COMMON_EARLTY_UNIT, earl::value::Type::Void},
};

bool earl::value::type_is_compatable(Obj *const obj1, Obj *const obj2) {

    // They are lists, make sure the lists hold the same types.
    if (obj1->type() == Type::List && obj2->type() == Type::List) {
        // List *const list1 = dynamic_cast<List *const>(obj1);
        // List *const list2 = dynamic_cast<List *const>(obj2);

        // if (list1->value().size() != list2->value().size()) {
        //     return false;
        // }

        // for (size_t i = 0; i < list1->value().size(); ++i) {
        //     if (!type_is_compatable(list1->value()[i], list2->value()[i])) {
        //         return false;
        //     }
        // }
        TODO("earl::value::type_is_compatable: List type compatibility");
        return true;
    }

    // Not list type, just check the primitive type
    if (obj1->type() == obj2->type()) {
        return true;
    }
    return false;
}
