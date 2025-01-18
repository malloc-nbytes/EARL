#ifndef REPL_H
#define REPL_H

#include <memory>

#include "repled.hpp"
#include "ctx.hpp"

namespace repl {
    std::shared_ptr<Ctx> run(std::vector<std::string> &include_dirs);
    void clearscrn(repled::RawInput& ri);
};

#endif // REPL_H
