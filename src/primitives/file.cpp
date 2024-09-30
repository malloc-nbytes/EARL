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
      m_stream(std::move(stream)), m_open(false), m_mode_actual(0) {
    const std::string &literal = m_mode->value();
    for (char c : literal) {
        switch (c) {
        case 'w': m_mode_actual |= static_cast<uint32_t>(Mode::Write); break;
        case 'r': m_mode_actual |= static_cast<uint32_t>(Mode::Read); break;
        case 'b': m_mode_actual |= static_cast<uint32_t>(Mode::Binary); break;
        default:
            std::string msg = "unknown mode `"+std::to_string(c)+"` for file type";
            throw InterpreterException(msg);
            break;
        }
    }
}

void
File::set_open(void) {
    m_open = true;
}

void
File::set_closed(void) {
    m_open = false;
}

void
File::dump(void) {
    if (!m_open) {
        std::string msg = "file is not open";
        throw InterpreterException(msg);
    }
    if ((m_mode_actual & static_cast<uint32_t>(Mode::Read)) == 0) {
        std::string msg = "file is not open for reading";
        throw InterpreterException(msg);
    }
    m_stream.seekg(0, std::ios::beg);
    std::cout << m_stream.rdbuf();
}

void
File::close(void) {
    if (!m_open) {
        std::string msg = "file is not open";
        throw InterpreterException(msg);
    }
    m_stream.close();
    this->set_closed();
}

std::shared_ptr<earl::value::Str>
File::read(void) {
    if (!m_open) {
        std::string msg = "file is not open";
        throw InterpreterException(msg);
    }
    if ((m_mode_actual & static_cast<uint32_t>(Mode::Read)) == 0) {
        std::string msg = "file is not open for reading";
        throw InterpreterException(msg);
    }
    m_stream.seekg(0, std::ios::beg);
    std::stringstream buf;
    buf << m_stream.rdbuf();
    return std::make_shared<earl::value::Str>(buf.str());
}

void
File::write(std::shared_ptr<Obj> value) {
    if (!m_open) {
        std::string msg = "file is not open";
        throw InterpreterException(msg);
    }

    if ((m_mode_actual & static_cast<uint32_t>(Mode::Write)) == 0) {
        std::string msg = "file is not open for writing";
        throw InterpreterException(msg);
    }

    switch (value->type()) {
    case Type::Int: {
        auto _int = dynamic_cast<Int *>(value.get());
        m_stream << _int->value();
    } break;
    case Type::Char: {
        auto _char = dynamic_cast<Char *>(value.get());
        m_stream << _char->value();
    } break;
    case Type::Str: {
        auto str = dynamic_cast<Str *>(value.get());
        m_stream << str->value();
    } break;
    default: {
        std::string msg = "cannot write `"+type_to_str(value->type())+"` type to a file";
        throw InterpreterException(msg);
    } break;
    }
}

void
File::writelines(std::shared_ptr<List> &value) {
    (void)value;
    UNIMPLEMENTED("File::writelines");
}

/*** OVERRIDES ***/
Type
File::type(void) const {
    return Type::File;
}

void
File::mutate(Obj *other, StmtMut *stmt) {
    (void)other;
    ASSERT_CONSTNESS(this, stmt);
    UNIMPLEMENTED("File::mutate");
}

std::shared_ptr<Obj>
File::copy(void) {
    UNIMPLEMENTED("File::copy");
}

std::string
File::to_cxxstring(void) {
    std::string msg = "File(fp="+m_fp->to_cxxstring()+", mode="+m_mode->to_cxxstring()+", stream=..., open="+std::to_string(m_open)+", mode_actual="+std::to_string(m_mode_actual)+")";
    return msg;
}
