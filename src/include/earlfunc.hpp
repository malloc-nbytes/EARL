#ifndef EARLFUNC_H
#define EARLFUNC_H

#include <memory>

#include "ast.hpp"
#include "earlty.hpp"
#include "earlvar.hpp"
#include "token.hpp"

struct EarlFunc {
  std::unique_ptr<Token> m_id;
  EarlTy::Type m_rettype;
  std::vector<std::unique_ptr<EarlVar>> m_args;
  std::unique_ptr<StmtBlock> m_block;

  EarlFunc(std::unique_ptr<Token> id,
           EarlTy::Type rettype,
           std::vector<std::unique_ptr<EarlVar>> args,
           std::unique_ptr<StmtBlock> block);

  ~EarlFunc() = default;
};

#endif // EARLFUNC_H
