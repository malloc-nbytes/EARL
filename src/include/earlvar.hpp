#ifndef EARLVAR_H
#define EARLVAR_H

#include <any>
#include <memory>

#include "token.hpp"
#include "earlty.hpp"

struct EarlVar {
  std::unique_ptr<Token> m_id;
  EarlTy::Type m_type;
  bool m_allocd;
  std::any m_value;

  uint32_t m_refcount;

  EarlVar(std::unique_ptr<Token> id, EarlTy::Type type, bool allocd,
          std::any value = nullptr, uint32_t refcount = 1);
};

#endif // EARLVAR_H
