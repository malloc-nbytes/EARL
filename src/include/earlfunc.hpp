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

#ifndef EARLFUNC_H
#define EARLFUNC_H

#include <memory>

#include "ast.hpp"
#include "earlty.hpp"
#include "earlvar.hpp"
#include "token.hpp"
#include "unique-scope.hpp"

namespace EarlFunc {
    enum class Attr {
        Pub = 1 << 0,
        World = 1 << 1,
    };

    struct Func {
        Token *m_id;
        EarlTy::Type m_rettype;
        std::vector<std::unique_ptr<EarlVar>> m_args;
        StmtBlock *m_block;

        std::vector<UniqueScope<std::string, EarlVar>> m_local_scope;

        Func(Token *id,
             EarlTy::Type rettype,
             std::vector<std::unique_ptr<EarlVar>> args,
             StmtBlock *block);

        ~Func() = default;

        void add_earlvar_to_local_scope(std::unique_ptr<EarlVar> var);
        EarlVar *get_earlvar_from_local_scope(const std::string &id);
        bool has_earlvar_in_local_scope(const std::string &id);
        void add_new_local_scope_context(void);

        void pop_scope(void);
        void push_scope(void);

        /*** DEBUG ***/
        void dump_local_scope(void);
    };
};

#endif // EARLFUNC_H
