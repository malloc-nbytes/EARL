#ifndef REPL_H
#define REPL_H

#include <memory>

#include "ctx.hpp"

namespace repl {
    std::shared_ptr<Ctx> run(std::vector<std::string> &include_dirs,
                             std::string *oneshot_output = nullptr,
                             std::string *oneshot_prompt = nullptr);
    std::string oneshot(std::string &prompt);
};

#endif // REPL_H
