#ifndef REPL_H
#define REPL_H

#include <memory>

#include "ctx.hpp"

namespace Repl {
    std::shared_ptr<Ctx> run(void);
};

#endif // REPL_H
