#ifndef EARLVAR_H
#define EARLVAR_H

#include <any>
#include <memory>

#include "token.hpp"
#include "earlty.hpp"

class EarlVar {
  std::unique_ptr<Token> m_id;
  EarlTy m_type;
  bool m_allocd;
  std::any m_value;

  uint32_t m_refcount;

public:
  EarlVar(std::unique_ptr<Token> id, EarlTy type, bool allocd,
          std::any value = nullptr, uint32_t refcount = 1);
};

#endif // EARLVAR_H
