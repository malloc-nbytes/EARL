#include "earl.hpp"
#include "err.hpp"
#include "common.hpp"

using namespace earl::function;

bool Obj::has_local(const std::string &id) {
    return m_local.contains(id);
}

