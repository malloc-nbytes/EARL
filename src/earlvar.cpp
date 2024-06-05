#include <any>
#include <memory>

#include "earlty.hpp"
#include "earlvar.hpp"

EarlVar::EarlVar(Token *id, EarlTy::Type type, bool allocd,
                 std::any value, uint32_t refcount)
    : m_id(id),
      m_type(type),
      m_allocd(allocd),
      m_value(value),
      m_refcount(refcount) {}

void EarlVar::set_value(std::any value) {
  m_value = value;
}