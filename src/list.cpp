#include <algorithm>
#include <cassert>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

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

Obj *List::rev(void) {
    std::reverse(m_value.begin(), m_value.end());
    return new Void();
}

Obj *List::append(std::vector<Obj *> &values) {
    // NOTE: To append a reference, use the below line
    // m_value.insert(m_value.end(), values.begin(), values.end());

    for (size_t i = 0; i < values.size(); ++i) {
        m_value.push_back(values[i]->copy());
    }

    return new Void();
}

Obj *List::binop(Token *op, Obj *other) {
    if (!type_is_compatable(this, other)) {
        return new Int(0);
    }

    switch (op->type()) {
    case TokenType::Plus: {
        auto list = new List(this->value());
        list->value().insert(list->value().end(),
                             dynamic_cast<List *>(other)->value().begin(),
                             dynamic_cast<List *>(other)->value().end());
        return list;
    } break;
    case TokenType::Double_Equals: {
        auto *other_lst = dynamic_cast<List *>(other);
        int res = 0;
        if (m_value.size() == other_lst->value().size()) {
            res = 1;
            for (size_t i = 0; i < m_value.size(); ++i) {
                Obj *o1 = this->value()[i];
                Obj *o2 = other_lst->value()[i];
                if (!type_is_compatable(o1, o2)) {
                    res = 0;
                    break;
                }
                if (o1->type() == Type::Int) {
                    if (dynamic_cast<Int *>(o1)->value() != dynamic_cast<Int *>(o2)->value()) {
                        res = 0;
                        break;
                    }
                }
                else if (o1->type() == Type::Str) {
                    if (dynamic_cast<Str *>(o1)->value() != dynamic_cast<Str *>(o2)->value()) {
                        res = 0;
                        break;
                    }
                }
                else {
                    res = 0;
                    break;
                }
            }
        }
        return new Int(res);
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
