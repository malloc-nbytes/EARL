#include <any>
#include <memory>

#include "earlty.hpp"
#include "earlvar.hpp"

EarlVar::EarlVar(std::unique_ptr<Token> id, EarlTy::Type type, bool allocd,
                 std::any value, uint32_t refcount)
    : m_id(std::move(id)),
      m_type(type),
      m_allocd(allocd),
      m_value(value),
      m_refcount(refcount) {}
