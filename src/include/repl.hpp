#ifndef REPL_H
#define REPL_H

#include <memory>

#include "ctx.hpp"
#include "repled.hpp"

enum class DebugStatus {
    None = 0,
    Step,
    Continue,
};

extern DebugStatus debug_status;
extern std::string REPL_HIST;
extern size_t LINENO;
extern std::vector<std::string> HIST;
extern repled::SS SS;
extern bool repl_debug_run;
extern repled::RawInput ri;
extern bool DEBUG_BREAKPOINT_HIT;

namespace repl {
    std::shared_ptr<Ctx> run(std::vector<std::string> &include_dirs, std::vector<std::string> &scripts);
    void print_value(std::shared_ptr<earl::value::Obj> &val, std::shared_ptr<Ctx> &ctx);
};

#endif // REPL_H
