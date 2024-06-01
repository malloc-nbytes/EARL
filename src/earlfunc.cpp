#include "ast.hpp"
#include "earlty.hpp"
#include "earlvar.hpp"
#include "token.hpp"
#include "earlfunc.hpp"

EarlFunc::EarlFunc(std::unique_ptr<Token> id,
                   EarlTy::Type rettype,
                   std::vector<std::unique_ptr<EarlVar>> args,
                   std::unique_ptr<StmtBlock> block)
    : m_id(std::move(id)),
      m_rettype(rettype),
      m_args(std::move(args)),
      m_block(std::move(block)) {}

