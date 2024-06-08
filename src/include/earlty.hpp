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

#ifndef EARLTY_H
#define EARLTY_H

#include <any>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

/// @brief Provides an interface to work with the
/// different types of EARL.
namespace EarlTy {

    /// @brief The different types of EARL.
    enum class Type {
        Int,
        Str,
        Void,
        List,
    };

    struct Int {
        int value;
    };

    struct Str {
        std::string value;
    };

    struct List {
        std::any elems;
    };

    /// @brief A map of C++ strings to the corrosponding EARL type.
    extern const std::unordered_map<std::string, Type> typemap;

    /// @brief A map from an EARL type to a vector of compatable EARL types.
    extern const std::unordered_map<EarlTy::Type, std::vector<EarlTy::Type>> m_earl_compat_tys;

    /// @brief Given EARL vars, checks to see if they
    /// are compatible.
    /// @param ty1 The first type
    /// @param ty2 The second type
    bool earlvar_type_compat(EarlTy::Type ty1, EarlTy::Type ty2);

    /// @brief Generate an EARL type from a string.
    Type of_str(const std::string &s);

    std::string to_string(Type ty);
};

#endif // EARLTY_H
