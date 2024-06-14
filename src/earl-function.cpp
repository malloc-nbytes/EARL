#include "earl.hpp"
#include "err.hpp"
#include "common.hpp"

using namespace earl::function;

Obj::Obj(StmtDef *stmtdef) : m_stmtdef(stmtdef) {}

const std::string &Obj::id(void) const {
    return m_stmtdef->m_id->lexeme();
}

bool Obj::has_local(const std::string &id) {
    return m_local.contains(id);
}

