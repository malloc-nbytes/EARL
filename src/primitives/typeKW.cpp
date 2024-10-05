/** @file */

// MIT License

// Copyright (c) 2023 malloc-nbytes

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cassert>
#include <memory>
#include <unordered_map>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

static const std::unordered_map<std::string, Type> typekw_map = {
    {"int", Type::Int},
    {"float", Type::Float},
    {"bool", Type::Bool},
    {"str", Type::Str},
    {"char", Type::Char},
    {"unit", Type::Void},
    {"list", Type::List},
    {"file", Type::File},
    {"option", Type::Option},
    {"closure", Type::Closure},
    {"tuple", Type::Tuple},
};

bool
earl::value::is_typekw(const std::string &id) {
    return typekw_map.find(id) != typekw_map.end();
}

// NOTE: It is expected to call earl::value::is_typekw before using this function.
Type
earl::value::get_typekw_proper(const std::string &id) {
    auto it = typekw_map.find(id);
    return it->second;
}

TypeKW::TypeKW(Type ty) : m_ty(ty) {}

Type
TypeKW::ty(void) const {
    return m_ty;
}

/*** OVERRIDES ***/
Type
TypeKW::type(void) const {
    return Type::TypeKW;
}

std::shared_ptr<Obj>
TypeKW::binop(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);

    if (op->type() == TokenType::Double_Equals)
        return std::make_shared<Bool>(m_ty == dynamic_cast<TypeKW *>(other)->ty());
    else if (op->type() == TokenType::Bang_Equals)
        return std::make_shared<Bool>(m_ty != dynamic_cast<TypeKW *>(other)->ty());

    Err::err_wtok(op);
    const std::string msg = "Invalid binary operation for type " + earl::value::type_to_str(m_ty);
    throw InterpreterException(msg);
}

void
TypeKW::mutate(Obj *other, StmtMut *stmt) {
    ASSERT_MUTATE_COMPAT(this, other, stmt);
    ASSERT_CONSTNESS(this, stmt);
    m_ty = dynamic_cast<TypeKW *>(other)->ty();
}

std::shared_ptr<Obj>
TypeKW::copy(void) {
    auto value = std::make_shared<TypeKW>(m_ty);
    if (m_info.has_value())
        value->set_info(m_info.value());
    return value;
}

bool
TypeKW::eq(Obj *other) {
    if (other->type() != Type::TypeKW)
        return false;
    return m_ty == dynamic_cast<TypeKW *>(other)->ty();
}

std::string
TypeKW::to_cxxstring(void) {
    return "<Type: " + earl::value::type_to_str(this->ty()) + ">";
}

std::shared_ptr<Obj>
TypeKW::equality(Token *op, Obj *other) {
    ASSERT_BINOP_COMPAT(this, other, op);

    if (op->type() == TokenType::Double_Equals)
        return std::make_shared<Bool>(m_ty == dynamic_cast<TypeKW *>(other)->ty());
    else if (op->type() == TokenType::Bang_Equals)
        return std::make_shared<Bool>(m_ty != dynamic_cast<TypeKW *>(other)->ty());

    Err::err_wtok(op);
    const std::string msg = "Invalid binary operation for type " + earl::value::type_to_str(m_ty);
    throw InterpreterException(msg);
}
