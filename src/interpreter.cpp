#include <cassert>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <any>

#include "interpreter.hpp"
#include "token.hpp"
#include "ast.hpp"
#include "common.hpp"

enum class EarlTy {
  Int,
  Str,
};

struct EarlVar {
  std::unique_ptr<Token> m_id;
  EarlTy m_type;
  bool m_allocd;
  std::any m_value;

  uint32_t m_refcount;

  EarlVar(std::unique_ptr<Token> id, EarlTy type, bool allocd,
          std::any value = nullptr, uint32_t refcount = 1)
    : m_id(std::move(id)),
      m_type(type),
      m_allocd(allocd),
      m_value(value),
      m_refcount(refcount) {}
};

struct Ctx {
  std::vector<std::unordered_map<std::string, EarlVar>> m_scope;
  std::unordered_map<EarlTy, std::vector<EarlTy>> m_earl_compat_tys;

  Ctx() {
    m_scope.emplace_back();

    m_earl_compat_tys[EarlTy::Int] = {EarlTy::Int};
    m_earl_compat_tys[EarlTy::Str] = {EarlTy::Str};
  }

  ~Ctx() = default;

  void add_earlvar(std::unique_ptr<Token> id, EarlTy type, bool allocd, std::any value = nullptr) {
    std::string name = id->lexeme();
    m_scope.back().emplace(name, EarlVar(std::move(id), type, allocd, std::move(value)));
  }

};

void scope_pop(Ctx &ctx) {
  ctx.m_scope.pop_back();
}

void scope_push(Ctx &ctx) {
  ctx.m_scope.emplace_back();
}

void interpret(Program &program)
{
  Ctx ctx;

  // for (size_t i = 0; i < program.stmts_len(); ++i) {
  //   eval_stmt(std::move(program.get_stmt(i)), ctx);
  // }
}
