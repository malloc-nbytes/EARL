#ifndef REPL_H
#define REPL_H

#include <memory>

#include "ctx.hpp"

namespace repl {
    std::shared_ptr<Ctx> run(std::vector<std::string> &include_dirs, std::vector<std::string> &scripts);
};

#endif // REPL_H
