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

#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#include <memory>

#include "earl.hpp"
#include "err.hpp"
#include "utils.hpp"

using namespace earl::value;

File::File(std::shared_ptr<Str> fp, std::shared_ptr<Str> mode, std::fstream stream)
    : m_fp(fp), m_mode(mode),
      m_stream(std::move(stream)), m_open(false) {}

void File::set_open(void) {
    m_open = true;
}

void File::set_closed(void) {
    m_open = false;
}

void File::dump(void) {
    if (!m_open) {
        ERR(Err::Type::Fatal, "file is not open");
    }

    if (m_mode->value() == "w") {
        ERR(Err::Type::Fatal, "file is not open for reading");
    }

    m_stream.seekg(0, std::ios::beg);

    std::cout << m_stream.rdbuf();
}

void File::close(void) {
    if (!m_open) {
        ERR(Err::Type::Fatal, "file is not open");
    }
    m_stream.close();
    this->set_closed();
}

std::shared_ptr<earl::value::Str> File::read(void) {
    UNIMPLEMENTED("File::read");
}

void File::write(std::shared_ptr<Obj> value) {
    (void)value;
    UNIMPLEMENTED("File::write");
}

void File::writelines(std::shared_ptr<List> &value) {
    (void)value;
    UNIMPLEMENTED("File::writelines");
}

/*** OVERRIDES ***/
Type File::type(void) const {
    return Type::File;
}

std::shared_ptr<Obj> File::binop(Token *op, std::shared_ptr<Obj> &other) {
    (void)op;
    (void)other;
    UNIMPLEMENTED("File::binop");
}

bool File::boolean(void) {
    UNIMPLEMENTED("File::boolean");
}

void File::mutate(const std::shared_ptr<Obj> &other) {
    (void)other;
    UNIMPLEMENTED("File::mutate");
}

std::shared_ptr<Obj> File::copy(void) {
    UNIMPLEMENTED("File::copy");
}

bool File::eq(std::shared_ptr<Obj> &other) {
    (void)other;
    UNIMPLEMENTED("File::eq");
}

std::string File::to_cxxstring(void) {
    ERR(Err::Type::Fatal, "unable to convert `file` type to a string");
}
