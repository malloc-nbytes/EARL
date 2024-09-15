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

#include "earl.hpp"
#include "utils.hpp"
#include "err.hpp"

using namespace earl::value;

std::shared_ptr<Obj>
Obj::binop(Token *op, Obj *other) {
    Err::err_wtok(op);
    const std::string msg = "cannot perform binary operator on type: `"+type_to_str(this->type())+"`";
    throw InterpreterException(msg);
}

bool
Obj::boolean(void) {
    const std::string msg = "cannot resolve boolean from type: `"+type_to_str(this->type())+"`";
    throw InterpreterException(msg);
}

void
Obj::mutate(Obj *other, StmtMut *stmt) {
    (void)other;
    Err::err_wstmt(stmt);
    const std::string msg = "unable to mutate value of type: `"+type_to_str(this->type())+"`";
    throw InterpreterException(msg);
}

std::shared_ptr<Obj>
Obj::copy(void) {
    const std::string msg = "unable to copy value of type: `"+type_to_str(this->type())+"`";
    throw InterpreterException(msg);
}

bool
Obj::eq(Obj *other) {
    (void)other;
    const std::string msg = "unable to perform deep-equality check on type: `"+type_to_str(this->type())+"`";
    throw InterpreterException(msg);
}

std::string
Obj::to_cxxstring(void) {
    const std::string msg = "unable to get cxxstring of value of type: `"+type_to_str(this->type())+"`";
    throw InterpreterException(msg);
}

void
Obj::spec_mutate(Token *op, Obj *other, StmtMut *stmt) {
    (void)other;
    (void)stmt;
    Err::err_wtok(op);
    const std::string msg = "cannot special mutate on type of: `"+type_to_str(this->type())+"`";
    throw InterpreterException(msg);
}

std::shared_ptr<Obj>
Obj::unaryop(Token *op) {
    Err::err_wtok(op);
    const std::string msg = "cannot use unary operator on type of: `"+type_to_str(this->type())+"`";
    throw InterpreterException(msg);
}

void
Obj::unset_const(void) {
    m_const = false;
}

bool
Obj::is_const(void) const {
    return m_const;
}

void
Obj::set_const(void) {
    m_const = true;
}

bool
Obj::is_iterable(void) const {
    return m_iterable;
}

Iterator
Obj::iter_begin(void) {
    const std::string msg = "value of type: `"+type_to_str(this->type())+"` is not iterable";
    throw InterpreterException(msg);
}

Iterator
Obj::iter_end(void) {
    const std::string msg = "value of type: `"+type_to_str(this->type())+"` is not iterable";
    throw InterpreterException(msg);
}

void
Obj::iter_next(Iterator &it) {
    const std::string msg = "value of type: `"+type_to_str(this->type())+"` is not iterable";
    throw InterpreterException(msg);
}

std::shared_ptr<Obj>
Obj::add(Token *op, Obj *other) {
    (void)other;
    Err::err_wtok(op);
    const std::string msg = "value of type: `"+type_to_str(this->type())+"` cannot be added";
    throw InterpreterException(msg);
}

std::shared_ptr<Obj>
Obj::sub(Token *op, Obj *other) {
    (void)other;
    Err::err_wtok(op);
    const std::string msg = "value of type: `"+type_to_str(this->type())+"` cannot be subtracted";
    throw InterpreterException(msg);
}

std::shared_ptr<Obj>
Obj::multiply(Token *op, Obj *other) {
    (void)other;
    Err::err_wtok(op);
    const std::string msg = "value of type: `"+type_to_str(this->type())+"` cannot be multiplied";
    throw InterpreterException(msg);
}

std::shared_ptr<Obj>
Obj::divide(Token *op, Obj *other) {
    (void)other;
    Err::err_wtok(op);
    const std::string msg = "value of type: `"+type_to_str(this->type())+"` cannot be divided";
    throw InterpreterException(msg);
}

std::shared_ptr<Obj>
Obj::modulo(Token *op, Obj *other) {
    (void)other;
    Err::err_wtok(op);
    const std::string msg = "value of type: `"+type_to_str(this->type())+"` cannot be modded";
    throw InterpreterException(msg);
}

std::shared_ptr<Obj>
Obj::power(Token *op, Obj *other) {
    (void)other;
    Err::err_wtok(op);
    const std::string msg = "value of type: `"+type_to_str(this->type())+"` cannot be set to a power";
    throw InterpreterException(msg);
}

std::shared_ptr<Obj>
Obj::gtequality(Token *op, Obj *other) {
    (void)other;
    Err::err_wtok(op);
    const std::string msg = "value of type: `"+type_to_str(this->type())+"` cannot be greaterthan/lessthan compared";
    throw InterpreterException(msg);
}

std::shared_ptr<Obj>
Obj::equality(Token *op, Obj *other) {
    (void)other;
    Err::err_wtok(op);
    const std::string msg = "value of type: `"+type_to_str(this->type())+"` cannot be checked for equality";
    throw InterpreterException(msg);
}

std::shared_ptr<Obj>
Obj::bitwise(Token *op, Obj *other) {
    (void)other;
    Err::err_wtok(op);
    const std::string msg = "value of type: `"+type_to_str(this->type())+"` cannot use bitwise operators";
    throw InterpreterException(msg);
}

std::shared_ptr<Obj>
Obj::bitshift(Token *op, Obj *other) {
    (void)other;
    Err::err_wtok(op);
    const std::string msg = "value of type: `"+type_to_str(this->type())+"` cannot be bit shifted";
    throw InterpreterException(msg);
}

