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

// File: ctx.hpp
// Description:
//   Provides a 'Context' struct that
//   holds all relevant information
//   that a runtime needs.

#ifndef CTX_H
#define CTX_H

#include <any>
#include <vector>
#include <unordered_map>

#include "earlty.hpp"
#include "earlvar.hpp"
#include "earlfunc.hpp"

class Ctx {
  // The scope of all runtime variables.
  std::vector<std::unordered_map<std::string, std::unique_ptr<EarlVar>>> m_scope;
  std::vector<std::unordered_map<std::string, std::unique_ptr<EarlFunc>>> m_functions;

public:
  Ctx();
  ~Ctx() = default;

  // Remove a new scope.
  void pop_scope(void);

  // Add a new scope.
  void push_scope(void);

  // Given `id`, checks if there is an
  // EarlVar in the current scope.
  bool earlvar_in_scope(const std::string &id);

  // Given `id` (indexes the scope by id->lexeme()),
  // the EARL type of `type`, whether or not it is
  // heap allocated with `allocd`, and the `value` that
  // the actual variable holds, will insert it into the
  // runtime scope.
  void create_and_add_earlvar_to_scope(std::unique_ptr<Token> id,
                                       EarlTy::Type type,
                                       bool allocd,
                                       std::any value);

  void add_earlvar_to_scope(std::unique_ptr<EarlVar> var);

  // Will get you the EarlVar that corrosponds
  // to `id`.
  EarlVar *get_earlvar_from_scope(const std::string &id);

  // Add an `EarlFunc` to the global function scope.
  void add_earlfunc_to_scope(std::unique_ptr<EarlFunc> func);

  // Another way to add an `EarlFunc` to the global function scope.
  // This one constructs it for you.
  void add_earlfunc_to_scope(std::unique_ptr<Token> id,
                             EarlTy::Type rettype,
                             std::vector<std::unique_ptr<EarlVar>> args,
                             std::unique_ptr<StmtBlock> block);

  bool earlfunc_in_scope(std::string &id);

  EarlFunc *get_earlfunc_from_scope(std::string &id);

  void debug_dump_earlfuncs(void);
  void debug_dump_earlvars(void);
};

#endif // CTX_H
