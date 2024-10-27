/** @file */

// MIT License

// Copyright (c) 2023 malloc-nbytes

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <unordered_map>

#include "earl.hpp"
#include "ctx.hpp"
#include "token.hpp"
#include "common.hpp"
#include "err.hpp"
#include "utils.hpp"

using BuiltinIdentSig = std::shared_ptr<earl::value::Obj> (*)(std::shared_ptr<Ctx>&);

std::shared_ptr<earl::value::Obj> builtin___FUNC__(std::shared_ptr<Ctx> &ctx);
std::shared_ptr<earl::value::Obj> builtin__FILE__(std::shared_ptr<Ctx> &ctx);
std::shared_ptr<earl::value::Obj> builtin__OS__(std::shared_ptr<Ctx> &ctx);
std::shared_ptr<earl::value::Obj> builtin__MODULE__(std::shared_ptr<Ctx> &ctx);

static const std::unordered_map<std::string, BuiltinIdentSig> builtin_idents = {
    {"__FUNC__", &builtin___FUNC__},
    {"__FILE__", &builtin__FILE__},
    {"__OS__", &builtin__OS__},
    {"__MODULE__", &builtin__MODULE__},
};

std::shared_ptr<earl::value::Obj>
builtin__MODULE__(std::shared_ptr<Ctx> &ctx) {
    auto world = ctx->get_world();
    return std::make_shared<earl::value::Str>(world->get_mod());
}

std::shared_ptr<earl::value::Obj>
builtin__OS__(std::shared_ptr<Ctx> &ctx) {
#if defined(_WIN32) || defined(_WIN64)
    return std::make_shared<earl::value::Str>("WINDOWS");
#elif defined(__APPLE__) || defined(__MACH__)
    return std::make_shared<earl::value::Str>("MAC");
#elif defined(__linux__)
    return std::make_shared<earl::value::Str>("LINUX");
#elif defined(__unix__)
    return std::make_shared<earl::value::Str>("UNIX");
#else
    return std::make_shared<earl::value::Str>("UNKNOWN");
#endif
}

std::shared_ptr<earl::value::Obj>
builtin__FILE__(std::shared_ptr<Ctx> &ctx) {
    if (ctx->type() == CtxType::World) {
        const std::string &id = dynamic_cast<WorldCtx *>(ctx.get())->get_filepath();
        return std::make_shared<earl::value::Str>(id);
    }
    if (ctx->type() == CtxType::Function) {
        auto wctx = dynamic_cast<FunctionCtx *>(ctx.get())->get_outer_world_owner();
        const std::string &id = dynamic_cast<WorldCtx *>(wctx.get())->get_filepath();
        return std::make_shared<earl::value::Str>(id);
    }
    if (ctx->type() == CtxType::Class) {
        auto wctx = dynamic_cast<ClassCtx *>(ctx.get())->get_owner();
        const std::string &id = dynamic_cast<WorldCtx *>(wctx.get())->get_filepath();
        return std::make_shared<earl::value::Str>(id);
    }
    if (ctx->type() == CtxType::Closure) {
        auto wctx = dynamic_cast<ClosureCtx *>(ctx.get())->get_outer_world_owner();
        const std::string &id = dynamic_cast<WorldCtx *>(wctx.get())->get_filepath();
        return std::make_shared<earl::value::Str>(id);
    }
    ERR_WARGS(Err::Type::Fatal, "unknown CTX type: ", (int)ctx->type());
    return nullptr; // unreachable
}

std::shared_ptr<earl::value::Obj>
builtin___FUNC__(std::shared_ptr<Ctx> &ctx) {
    if (ctx->type() == CtxType::Function) {
        const std::string &id = dynamic_cast<FunctionCtx *>(ctx.get())->get_curfuncid();
        return std::make_shared<earl::value::Str>(id);
    }
    return std::make_shared<earl::value::Str>("");
}

bool
earl::value::is_builtin_ident(const std::string &id) {
    return builtin_idents.find(id) != builtin_idents.end();
}

std::shared_ptr<earl::value::Obj>
earl::value::get_builtin_ident(const std::string &id, std::shared_ptr<Ctx> &ctx) {
    auto it = builtin_idents.find(id);
    if (it != builtin_idents.end())
        return it->second(ctx);
    assert(false && "invalid builtin ident. must call is_builtin_ident() first");
    return nullptr; // unreachable
}

