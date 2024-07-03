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

#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_map>
#include <fstream>

#include "intrinsics.hpp"
#include "interpreter.hpp"
#include "err.hpp"
#include "ctx.hpp"
#include "ast.hpp"
#include "earl.hpp"
#include "utils.hpp"

const std::unordered_map<std::string, Intrinsics::IntrinsicFunction> Intrinsics::intrinsic_functions = {
    {"print", &Intrinsics::intrinsic_print},
    {"assert", &Intrinsics::intrinsic_assert},
    {"len", &Intrinsics::intrinsic_len},
    {"open", &Intrinsics::intrinsic_open},
    {"type", &Intrinsics::intrinsic_type},
    {"unimplemented", &Intrinsics::intrinsic_unimplemented},
    {"some", &Intrinsics::intrinsic_some},
    {"__internal_move__", &Intrinsics::intrinsic___internal_move__},
};

const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction> Intrinsics::intrinsic_member_functions = {
    {"nth", &Intrinsics::intrinsic_member_nth},
    {"filter", &Intrinsics::intrinsic_member_filter},
    {"rev", &Intrinsics::intrinsic_member_rev},
    {"append", &Intrinsics::intrinsic_member_append},
    {"pop", &Intrinsics::intrinsic_member_pop},
    {"split", &Intrinsics::intrinsic_member_split},
    {"substr", &Intrinsics::intrinsic_member_substr},
    {"remove_lines", &Intrinsics::intrinsic_member_remove_lines},
    {"dump", &Intrinsics::intrinsic_member_dump},
    {"close", &Intrinsics::intrinsic_member_close},
    {"read", &Intrinsics::intrinsic_member_read},
    {"ascii", &Intrinsics::intrinsic_member_ascii},
    {"unwrap", &Intrinsics::intrinsic_member_unwrap},
    {"is_none", &Intrinsics::intrinsic_member_is_none},
    {"is_some", &Intrinsics::intrinsic_member_is_some},
};

earl::value::Obj *Intrinsics::call(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    const std::string &id = expr->m_id->lexeme();
    return Intrinsics::intrinsic_functions.at(id)(expr, params, ctx);
}

bool Intrinsics::is_intrinsic(const std::string &id) {
    return Intrinsics::intrinsic_functions.find(id) != Intrinsics::intrinsic_functions.end();
}

bool Intrinsics::is_member_intrinsic(const std::string &id) {
    return Intrinsics::intrinsic_member_functions.find(id) != Intrinsics::intrinsic_member_functions.end();
}

earl::value::Obj *Intrinsics::call_member(const std::string &id, earl::value::Obj *accessor, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    return Intrinsics::intrinsic_member_functions.at(id)(accessor, params, ctx);
}

earl::value::Obj *Intrinsics::intrinsic_member_split(earl::value::Obj *obj, std::vector<earl::value::Obj *> &delim, Ctx &ctx) {
    (void)ctx;

    if (delim.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`split` member intrinsic expects 1 arguments but %zu were supplied",
                  delim.size());
    }

    if (obj->type() != earl::value::Type::Str) {
        ERR(Err::Type::Fatal, "`split` member intrinsic is only defined for `str` types");
    }

    earl::value::Str *str = dynamic_cast<earl::value::Str *>(obj);
    return str->split(delim[0]);
}

earl::value::Obj *Intrinsics::intrinsic_member_substr(earl::value::Obj *obj, std::vector<earl::value::Obj *> &idxs, Ctx &ctx) {
    (void)ctx;

    earl::value::Obj *tmp = obj;

    if (idxs.size() != 2) {
        ERR_WARGS(Err::Type::Fatal, "`substr` member intrinsic expects 2 arguments but %zu were supplied",
                  idxs.size());
    }

    if (obj->type() != earl::value::Type::Str) {
        ERR(Err::Type::Fatal, "`substr` member intrinsic is only defined for `str` types");
    }

    if (idxs[0]->type() != earl::value::Type::Int) {
        ERR(Err::Type::Fatal, "argument 1 in `substr` expects an `int` value");
    }

    if (idxs[1]->type() != earl::value::Type::Int) {
        ERR(Err::Type::Fatal, "argument 2 in `substr` expects an `int` value");
    }

    auto *idx1 = dynamic_cast<earl::value::Int *>(idxs[0]);
    auto *idx2 = dynamic_cast<earl::value::Int *>(idxs[1]);
    return dynamic_cast<earl::value::Str *>(tmp)->substr(idx1, idx2);
}

earl::value::Obj *Intrinsics::intrinsic_member_read(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    (void)ctx;

    if (unused.size() != 0) {
        ERR_WARGS(Err::Type::Fatal, "`read` member intrinsic expects 0 arguments but %zu were supplied",
                  unused.size());
    }

    if (obj->type() != earl::value::Type::File) {
        ERR(Err::Type::Fatal, "`read` member intrinsic is only defined for `file` types");
    }

    auto *f = dynamic_cast<earl::value::File *>(obj);
    return f->read();
}

earl::value::Obj *Intrinsics::intrinsic_member_ascii(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    (void)ctx;

    if (unused.size() != 0) {
        ERR_WARGS(Err::Type::Fatal, "`ascii` member intrinsic expects 0 arguments but %zu were supplied",
                  unused.size());
    }

    if (obj->type() != earl::value::Type::Char) {
        ERR(Err::Type::Fatal, "`ascii` member intrinsic is only defined for `char` types");
    }

    return
        new earl::value::Int(static_cast<int>(dynamic_cast<earl::value::Char *>(obj)->value()));
}

earl::value::Obj *Intrinsics::intrinsic_member_nth(earl::value::Obj *obj, std::vector<earl::value::Obj *> &idx, Ctx &ctx) {
    (void)ctx;

    if (idx.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`nth` member intrinsic expects 1 argument but %zu were supplied",
                  idx.size());
    }

    if (obj->type() == earl::value::Type::List) {
        earl::value::List *list = dynamic_cast<earl::value::List *>(obj);
        return list->nth(idx[0]);
    }
    else if (obj->type() == earl::value::Type::Str) {
        earl::value::Str *str = dynamic_cast<earl::value::Str *>(obj);
        return str->nth(idx[0]);
    }
    else {
        ERR(Err::Type::Fatal, "`nth` member intrinsic is only defined for `list` and `str` types");
    }
}

earl::value::Obj *Intrinsics::intrinsic_member_filter(earl::value::Obj *obj, std::vector<earl::value::Obj *> &closure, Ctx &ctx) {
    if (closure.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`filter` member intrinsic expects 1 argument but %zu were supplied",
                  closure.size());
    }

    if (closure[0]->type() != earl::value::Type::Closure) {
        ERR(Err::Type::Fatal, "argument 1 in `filter` expects a `closure` value");
    }

    return dynamic_cast<earl::value::List *>(obj)->filter(closure.at(0), ctx);
}

earl::value::Obj *Intrinsics::intrinsic_member_rev(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    (void)ctx;

    if (unused.size() != 0) {
        ERR_WARGS(Err::Type::Fatal, "`rev` member intrinsic expects 0 arguments but %zu were supplied",
                  unused.size());
    }

    if (obj->type() != earl::value::Type::List) {
        ERR(Err::Type::Fatal, "`rev` member intrinsic is only defined for `list` types");
    }

    auto *list = dynamic_cast<earl::value::List *>(obj);
    list->rev();

    return new earl::value::Void();
}

earl::value::Obj *Intrinsics::intrinsic_member_append(earl::value::Obj *obj, std::vector<earl::value::Obj *> &values, Ctx &ctx) {
    (void)ctx;

    if (values.size() == 0) {
        ERR_WARGS(Err::Type::Fatal, "`append` member intrinsic expects variadic arguments but %zu were supplied",
                  values.size());
    }

    if (obj->type() != earl::value::Type::List) {
        ERR(Err::Type::Fatal, "`append` member intrinsic is only defined for `list` types");
    }

    auto *lst = dynamic_cast<earl::value::List *>(obj);
    return lst->append(values);
}

earl::value::Obj *Intrinsics::intrinsic_member_pop(earl::value::Obj *obj, std::vector<earl::value::Obj *> &values, Ctx &ctx) {
    (void)ctx;

    if (values.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`pop` member intrinsic expects 1 argument but %zu were supplied",
                  values.size());
    }

    if (obj->type() != earl::value::Type::List && obj->type() != earl::value::Type::Str) {
        ERR(Err::Type::Fatal, "`pop` member intrinsic is only defined for `list` and `str` types");
    }

    if (obj->type() == earl::value::Type::List) {
        auto *lst = dynamic_cast<earl::value::List *>(obj);
        return lst->pop(values[0]);
    }
    else if (obj->type() == earl::value::Type::Str) {
        auto *str = dynamic_cast<earl::value::Str *>(obj);
        return str->pop(values[0]);
    }

    assert(false && "unreachable");
}

earl::value::Obj *Intrinsics::intrinsic_member_dump(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    (void)ctx;

    if (unused.size() != 0) {
        ERR(Err::Type::Fatal, "`dump` member intrinsic expects 0 arguments");
    }

    if (obj->type() != earl::value::Type::File) {
        ERR(Err::Type::Fatal, "`dump` member intrinsic is only defined for `file` types");
    }

    auto *f = dynamic_cast<earl::value::File *>(obj);
    f->dump();

    return new earl::value::Void();
}

earl::value::Obj *Intrinsics::intrinsic_len(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)expr;
    (void)ctx;

    if (params.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`len` intrinsic expects 1 argument but %zu were supplied",
                  params.size());
    }

    if (params[0]->type() != earl::value::Type::List && params[0]->type() != earl::value::Type::Str) {
        ERR(Err::Type::Fatal, "`len` intrinsic is only defined for `list` and `str` types");
    }

    if (params[0]->type() == earl::value::Type::List) {
        return new earl::value::Int(dynamic_cast<earl::value::List *>(params[0])->value().size());
    }
    else if (params[0]->type() == earl::value::Type::Str) {
        return new earl::value::Int(dynamic_cast<earl::value::Str *>(params[0])->value().size());
    }

    ERR_WARGS(Err::Type::Fatal, "canot call `len` on unsupported type (%d)", (int)params[0]->type());
    return nullptr;
}

earl::value::Obj *Intrinsics::intrinsic_some(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    if (params.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`some` intrinsic expects 1 argument but %zu were supplied",
                  params.size());
    }

    return new earl::value::Option(params[0]);
}

earl::value::Obj *Intrinsics::intrinsic___internal_move__(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    if (params.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`__internal_move__` intrinsic expects 1 argument but %zu were supplied",
                  params.size());
    }

    auto *ret = params[0]->copy();
    delete params[0];
    return ret;
}

earl::value::Obj *Intrinsics::intrinsic_type(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)expr;
    (void)ctx;

    if (params.size() != 1) {
        ERR_WARGS(Err::Type::Fatal, "`type` intrinsic expects 1 argument but %zu were supplied",
                  params.size());
    }

    return new earl::value::Str(earl::value::type_to_str(params[0]));
}

earl::value::Obj *Intrinsics::intrinsic_unimplemented(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    std::cerr << "[EARL] UNIMPLEMENTED";

    if (params.size() != 0) {
        std::cerr << ": ";
        Intrinsics::intrinsic_print(expr, params, ctx);
    }

    exit(1);

    return nullptr; // unreachable
}

earl::value::Obj *Intrinsics::intrinsic_assert(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)ctx;

    for (size_t i = 0; i < params.size(); ++i) {
        earl::value::Obj *param = params.at(i);
        if (!param->boolean()) {
            Err::err_wtok(expr->m_id.get());
            token_dump_until_eol(expr->m_id.get());
            ERR_WARGS(Err::Type::Assertion,
                      "assertion failure (expression=%zu) (earl::value::Type=%d)",
                      i+1, (int)param->type());
        }
    }

    return new earl::value::Void();
}

static void __intrinsic_print(ExprFuncCall *expr, earl::value::Obj *param) {
    switch (param->type()) {
    case earl::value::Type::Int: {
        auto *intparam = dynamic_cast<earl::value::Int *>(param);
        std::cout << intparam->value();
    } break;
    case earl::value::Type::Bool: {
        auto *bool_param = dynamic_cast<earl::value::Bool *>(param);
        std::cout << (bool_param->value() ? "true" : "false");
    } break;
    case earl::value::Type::Option: {
        auto *option_param = dynamic_cast<earl::value::Option *>(param);

        if (option_param->is_none())
            std::cout << "<none>";
        else {
            std::cout << "some(";
            __intrinsic_print(expr, option_param->value());
            std::cout << ")";
        }

    } break;
    case earl::value::Type::Str: {
        auto *strparam = dynamic_cast<earl::value::Str *>(param);
        std::cout << strparam->value();
    } break;
    case earl::value::Type::Char: {
        auto *strparam = dynamic_cast<earl::value::Char *>(param);
        std::cout << strparam->value();
    } break;
    case earl::value::Type::Class: {
        auto *classparam = dynamic_cast<earl::value::Class *>(param);
        std::cout << "<Class " << classparam->id() << " " << &classparam << '>';
    } break;
    case earl::value::Type::List: {
        earl::value::List *listparam = dynamic_cast<earl::value::List *>(param);
        std::cout << '[';
        std::vector<earl::value::Obj *> &list = listparam->value();

        for (size_t i = 0; i < list.size(); ++i) {
            __intrinsic_print(expr, list[i]);

            if (i != list.size()-1) {
                std::cout << ", ";
            }
        }

        std::cout << ']';
    } break;
    default: {
        Err::err_wtok(expr->m_id.get());
        ERR_WARGS(Err::Type::Fatal, "intrinsic_print: unknown parameter type %d", static_cast<int>(param->type()));
    } break;
    }
}

earl::value::Obj *Intrinsics::intrinsic_print(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)ctx;
    for (size_t i = 0; i < params.size(); ++i) {
        __intrinsic_print(expr, params[i]);
    }
    std::cout << '\n';
    return new earl::value::Void();
}

earl::value::Obj *Intrinsics::intrinsic_member_remove_lines(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    UNIMPLEMENTED("Intrinsics::intrinsic_member_remove_lines");

    (void)ctx;
    assert(obj->type() == earl::value::Type::Str);
    assert(unused.size() == 0);
    auto *str = dynamic_cast<earl::value::Str *>(obj);
    return str->remove_lines();
}

earl::value::Obj *Intrinsics::intrinsic_open(ExprFuncCall *expr, std::vector<earl::value::Obj *> &params, Ctx &ctx) {
    (void)ctx;
    (void)expr;

    if (params.size() != 2) {
        ERR_WARGS(Err::Type::Fatal, "`open` intrinsic expects 2 arguments but %zu were supplied",
                  params.size());
    }

    if (params[0]->type() != earl::value::Type::Str) {
        ERR(Err::Type::Fatal, "argument 1 in `open` expects a `str` value");
    }
    if (params[1]->type() != earl::value::Type::Str) {
        ERR(Err::Type::Fatal, "argument 2 in `open` expects a `str` value");
    }

    auto *fp = dynamic_cast<earl::value::Str *>(params[0]);
    auto *mode = dynamic_cast<earl::value::Str *>(params[1]);
    std::fstream stream;

    if (mode->value() == "r") {
        stream.open(fp->value(), std::ios::in);
    }
    else if (mode->value() == "w") {
        stream.open(fp->value(), std::ios::out);
    }
    else {
        ERR_WARGS(Err::Type::Fatal, "invalid mode `%s` for file handler, must be either r|w",
                  mode->value().c_str());
    }

    if (!stream) {
        ERR_WARGS(Err::Type::Fatal, "file `%s` could not be found", fp->value().c_str());
    }

    auto *f = new earl::value::File(fp, mode, std::move(stream));
    f->set_open();

    return f;
}

earl::value::Obj *Intrinsics::intrinsic_member_close(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    (void)ctx;

    if (unused.size() != 0) {
        ERR_WARGS(Err::Type::Fatal, "`close` member intrinsic expects 0 arguments but %zu were supplied",
                  unused.size());
    }

    if (obj->type() != earl::value::Type::File) {
        ERR(Err::Type::Fatal, "`close` member intrinsic is only defined for `file` types");
    }

    auto *f = dynamic_cast<earl::value::File *>(obj);
    f->close();
    return new earl::value::Void();
}

earl::value::Obj *Intrinsics::intrinsic_member_unwrap(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    if (unused.size() != 0) {
        ERR_WARGS(Err::Type::Fatal, "`unwrap` member intrinsic expects 0 arguments but %zu were supplied",
                  unused.size());
    }

    if (obj->type() != earl::value::Type::Option) {
        ERR(Err::Type::Fatal, "`unwrap` member intrinsic is only defined for `option` types");
    }

    auto *none = dynamic_cast<earl::value::Option *>(obj);

    if (!none->value()) {
        ERR(Err::Type::Fatal, "tried to unwrap none value");
    }

    return none->value();
}


earl::value::Obj *Intrinsics::intrinsic_member_is_none(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    if (unused.size() != 0) {
        ERR_WARGS(Err::Type::Fatal, "`is_none` member intrinsic expects 0 arguments but %zu were supplied",
                  unused.size());
    }

    if (obj->type() != earl::value::Type::Option) {
        ERR(Err::Type::Fatal, "`is_none` member intrinsic is only defined for `option` types");
    }

    return new earl::value::Bool(dynamic_cast<earl::value::Option *>(obj)->is_none());
}

earl::value::Obj *Intrinsics::intrinsic_member_is_some(earl::value::Obj *obj, std::vector<earl::value::Obj *> &unused, Ctx &ctx) {
    if (unused.size() != 0) {
        ERR_WARGS(Err::Type::Fatal, "`is_some` member intrinsic expects 0 arguments but %zu were supplied",
                  unused.size());
    }

    if (obj->type() != earl::value::Type::Option) {
        ERR(Err::Type::Fatal, "`is_some` member intrinsic is only defined for `option` types");
    }

    return new earl::value::Bool(dynamic_cast<earl::value::Option *>(obj)->is_some());
}
