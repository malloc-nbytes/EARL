#include <cassert>
#include <any>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "earlty.hpp"
#include "earlvar.hpp"
#include "err.hpp"
#include "ctx.hpp"

Ctx::Ctx() {
    m_cur_earlfunc = nullptr;
}

void Ctx::set_current_earlfunc(EarlFunc::Func *func) {
    assert(false);
}

void Ctx::unset_current_earlfunc(void) {
    assert(false);
}

void Ctx::pop_scope(void) {
    assert(false);
}

void Ctx::push_scope(void) {
    assert(false);
}
