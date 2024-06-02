#include "ast.hpp"
#include "earlty.hpp"
#include "earlvar.hpp"
#include "token.hpp"
#include "earlfunc.hpp"

EarlFunc::EarlFunc(Token *id,
                   EarlTy::Type rettype,
                   std::vector<std::unique_ptr<EarlVar>> args,
                   StmtBlock *block)
    : m_id(id),
      m_rettype(rettype),
      m_args(std::move(args)),
      m_block(block) {}

