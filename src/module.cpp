#include <cassert>

#include "earl.hpp"
#include "utils.hpp"

using namespace earl::value;

Module::Module(Ctx *ctx) : m_value(ctx) {}

Ctx *Module::value(void) {
    assert(m_value);
    return m_value;
}

Type Module::type(void) const {
    return Type::Module;
}

Obj *Module::binop(Token *op, Obj *other) {
    UNIMPLEMENTED("Module::binop");
}

bool Module::boolean(void) {
    UNIMPLEMENTED("Module::boolean");
}

void Module::mutate(Obj *other) {
    UNIMPLEMENTED("Module::mutate");
}

Obj *Module::copy(void) {
    UNIMPLEMENTED("Module::copy");
}
