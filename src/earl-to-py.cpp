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

// *** TODO: Update bitwise binary operators for Python ***

#include <iostream>
#include <cassert>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "err.hpp"
#include "ast.hpp"
#include "intrinsics.hpp"

#define PYSTMT(s, ctx)                          \
    do {                                        \
        tabs(ctx);                              \
        ctx.py_src += s "\n";                   \
    } while (0)

#define PYAPPEND(s, ctx)                        \
    do {                                        \
        ctx.py_src += s;                        \
    } while (0)

#define PYSTMT_CONS(s, ctx)                     \
    do {                                        \
        tabs(ctx);                              \
        ctx.py_src += s+"\n";                   \
    } while (0)

#define PYTAB "    "
#define PY_INTRINSIC_REPLACEMENT_CODE '~'

using etopy_intr = std::pair<std::string, std::optional<const std::string>>;

struct etopy {
    std::string py_repl;
    std::vector<std::string> extra_py_params;
    std::vector<std::string> imports;
    etopy(std::string py_repl, std::vector<std::string> extra_py_params,
          std::vector<std::string> imports)
        : py_repl(py_repl), extra_py_params(extra_py_params), imports(imports) {
    }
};

struct Context {
    unsigned scope_depth;
    std::string py_src;
    bool in_class;
    std::vector<std::string> lambdas;
    std::vector<std::string> earl_imports;
    std::vector<std::string> py_imports;
    std::vector<std::pair<std::string, std::string>> py_member_intrinsics;
};

static unsigned lambdas_count = 0;

static std::string expr_to_py(Expr *expr, Context &ctx);
static void stmt_block_to_py(StmtBlock *stmt, Context &ctx);
static void stmt_to_py(Stmt *stmt, Context &ctx);

static const std::unordered_set<std::string>
intrinsic_member_functions_exceptions = {
    "pop",
    "split",
};

// static const std::unordered_map<std::string, etopy> earl_to_py_equiv = {
//     {"println", etopy("print(~)", )}
// };

//                                  ID     PyID/import needed
static const std::unordered_map<std::string, etopy_intr>
intrinsic_function_python_equiv = {
    {"println", etopy_intr("print(~sep='')",           {})},
    {"print",   etopy_intr("print(~sep='', end='')",   {})},
    {"input",   etopy_intr("input(~)",                 {})},
    {"int",     etopy_intr("int(~)",                   {})},
    {"float",   etopy_intr("float(~)",                 {})},
    {"str",     etopy_intr("str(~)",                   {})},
    {"bool",    etopy_intr("bool(~)",                  {})},
    {"tuple",   etopy_intr("(~,)",                     {})},
    {"list",    etopy_intr("[~]",                      {})},
    {"Dict",    etopy_intr("dict()",                   {})},
    {"len",     etopy_intr("len(~)",                   {})},
    {"some",    etopy_intr("~",                        {})},
    {"type",    etopy_intr("type(~).__name__",         {})},
    {"typeof",  etopy_intr("type(~)",                  {})},
    {"argv",    etopy_intr("sys.argv",                 "sys")},
    {"open",    etopy_intr("open(~)",                  {})},
    {"unimplemented", etopy_intr("sys.exit()",         "sys")},
    {"sleep",   etopy_intr("time.sleep(~*1000)",       "time")},
    {"env",     etopy_intr("os.getenv(~)",             "os")},
    {"datetime", etopy_intr("datetime.datetime.now()", "datetime")},
};

static void
add_ctx_py_import(const std::string &py_import, Context &ctx) {
    for (auto &i : ctx.py_imports) {
        if (i == py_import)
            return;
    }
    ctx.py_imports.push_back(py_import);
}

static std::string
earl_intrinsic_to_py_intrinsic(const std::string &orig_funccall,
                               const std::string &comma_sep_params_str,
                               Context &ctx) {
    auto it = intrinsic_function_python_equiv.find(orig_funccall);

    // Function was called without checking first.
    if (it == intrinsic_function_python_equiv.end()) {
        std::cerr <<
            "EARL intrinsic `"+orig_funccall+"` does not have an equivalent Python intrinsic"
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const std::string &py_replacement = it->second.first;
    std::string py_intr_buf = "";
    bool found_replacement_pos = false;

    for (size_t i = 0; i < py_replacement.size(); ++i) {
        char ch = py_replacement.at(i);
        if (ch == PY_INTRINSIC_REPLACEMENT_CODE) {
            if (found_replacement_pos) {
                std::cerr <<
                    "can only have 1 replacement location in Python intrinsic"
                          << std::endl;
                std::exit(EXIT_FAILURE);
            }
            found_replacement_pos = true;
            py_intr_buf += comma_sep_params_str;
            if (comma_sep_params_str != "")
                py_intr_buf += ", ";
        }
        else
            py_intr_buf += ch;
    }

    // There is an import that we need
    if (it->second.second.has_value())
        add_ctx_py_import(it->second.second.value(), ctx);

    return py_intr_buf;
}

void tabs(Context &ctx) {
    for (unsigned i = 0; i < ctx.scope_depth; ++i) {
        ctx.py_src += PYTAB;
    }
}

bool ident_is_intrinsic_function(const std::string &id) {
    const std::unordered_map<std::string, Intrinsics::IntrinsicFunction> &map =
        Intrinsics::intrinsic_functions;
    return map.find(id) != map.end();
}

bool ident_is_intrinsic_member_function(const std::string &id) {
    const std::unordered_map<std::string, Intrinsics::IntrinsicMemberFunction> &map =
        Intrinsics::intrinsic_member_functions;
    return map.find(id) != map.end();
}

static std::string
expr_term_ident_to_py(ExprIdent *expr, Context &ctx) {
    std::string id = expr->m_tok->lexeme();
    if (ctx.in_class && id == "this")
        id = "self";
    return id;
}

static std::string
expr_term_intlit_to_py(ExprIntLit *expr, Context &ctx) {
    return expr->m_tok->lexeme();
}

static std::string
expr_term_strlit_to_py(ExprStrLit *expr, Context &ctx) {
    std::string pystr = "";
    for (size_t i = 0; i < expr->m_tok->lexeme().size(); ++i) {
        char ch = expr->m_tok->lexeme()[i];
        if (ch == '\n')
            pystr += "\\n";
        else if (ch == '\t')
            pystr += "\\t";
        else if (ch == '\r')
            pystr += "\\r";
        else
            pystr += ch;
    }
    return "\""+pystr+"\"";
}

static std::string
expr_term_charlit_to_py(ExprCharLit *expr, Context &ctx)  {
    return "'" + expr->m_tok->lexeme() + "'";
}

static std::string
expr_term_floatlit_to_py(ExprFloatLit *expr, Context &ctx)  {
    return expr->m_tok->lexeme();
}

static std::string
expr_term_funccall_to_py(ExprFuncCall *expr, Context &ctx) {
    std::string accessor = expr_to_py(expr->m_left.get(), ctx);

    std::string params = "";
    for (size_t i = 0; i < expr->m_params.size(); ++i) {
        params += expr_to_py(expr->m_params.at(i).get(), ctx);
        if (i != expr->m_params.size() - 1)
            params += ", ";
    }

    if (ident_is_intrinsic_function(accessor)) {
        accessor = earl_intrinsic_to_py_intrinsic(accessor, params, ctx);
        return accessor;
    }

    return accessor+"("+params+")";
}

static std::string
expr_term_listlit_to_py(ExprListLit *expr, Context &ctx) {
    std::string exprs = "[";
    for (size_t i = 0; i < expr->m_elems.size(); ++i) {
        exprs += expr_to_py(expr->m_elems.at(i).get(), ctx);
        if (i != expr->m_elems.size() - 1)
            exprs += ", ";
    }
    return exprs+"]";
}

static std::string
expr_term_get_to_py(ExprGet *expr, Context &ctx) {
    std::string left = expr_to_py(expr->m_left.get(), ctx);
    std::string right = "";

    std::visit([&](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::unique_ptr<ExprIdent>>)
            right = expr_to_py(arg.get(), ctx);
        else if constexpr (std::is_same_v<T, std::unique_ptr<ExprFuncCall>>)
            right = expr_to_py(arg.get(), ctx);
        else {
            std::string msg = "A serious internal error has ocured and has gotten to an unreachable case. Something is very wrong";
            throw InterpreterException(msg);
        }
    }, expr->m_right);

    std::string ident = "";
    std::string new_right = "";
    size_t open_paren = 0;
    size_t close_paren = 0;
    for (size_t i = 0; i < right.size(); ++i) {
        if (right.at(i) == '(') {
            for (size_t j = i; j < right.size(); ++j)
                new_right += right.at(j);
            break;
        }
        ident += right.at(i);
    }

    return left+"."+right;
}

static std::string
expr_term_mod_to_py_access(ExprModAccess *expr, Context &ctx) {
    const std::string pyleft = expr_to_py(expr->m_expr_ident.get(), ctx);
    std::string pyright = "";
    std::visit([&](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::unique_ptr<ExprIdent>>)
            pyright = expr_to_py(arg.get(), ctx);
        else if constexpr (std::is_same_v<T, std::unique_ptr<ExprFuncCall>>)
            pyright = expr_to_py(arg.get(), ctx);
        else {
            std::string msg = "A serious internal error has ocured and has gotten to an unreachable case. Something is very wrong";
            throw InterpreterException(msg);
        }
    }, expr->m_right);

    return pyleft+"."+pyright;
}

static std::string
expr_term_array_access_to_py(ExprArrayAccess *expr, Context &ctx) {
    const std::string pyleft = expr_to_py(expr->m_left.get(), ctx);
    const std::string pyidx = expr_to_py(expr->m_expr.get(), ctx);
    return pyleft+"["+pyidx+"]";
}

static std::string
expr_term_boollit_to_py(ExprBool *expr, Context &context)  {
    return expr->m_value ? "true" : "false";
}

static std::string
expr_term_none_to_py(ExprNone *expr, Context &context)  {
    return "None";
}

static std::string construct_closure(ExprClosure *expr, Context &ctx) {
    std::string lambda_name = "__EARL_CLOSURE__" + std::to_string(lambdas_count++);
    std::string pylambda = "def " + lambda_name + "(";
    for (size_t i = 0; i < expr->m_args.size(); ++i) {
        pylambda += expr->m_args.at(i).first->lexeme();
        if (i != expr->m_args.size()-1)
            pylambda += ", ";
    }
    pylambda += "):\n";

    Context clctx = {0, pylambda, false, {}, {}, {}};
    stmt_block_to_py(expr->m_block.get(), clctx);

    ctx.lambdas.push_back(clctx.py_src);

    for (size_t i = 0; i < clctx.lambdas.size(); ++i)
        ctx.lambdas.push_back(clctx.lambdas.at(i));

    return lambda_name;
}

static std::string
expr_term_closure_to_py(ExprClosure *expr, Context &ctx) {
    return construct_closure(expr, ctx);
}

static std::string
expr_term_range_to_py(ExprRange *expr, Context &ctx) {
    std::string pylst = "[";
    const std::string pystart = expr_to_py(expr->m_start.get(), ctx);
    const std::string pyend = expr_to_py(expr->m_end.get(), ctx);
    bool ischar = false;
    int start = 0;
    int end = 0;

    if (pystart[0] == '\'') {
        start = pystart[1];
        end = pyend[1];
        ischar = true;
    }
    else {
        start = std::stoi(pystart);
        end = std::stoi(pyend);
    }

    for (int i = start; i < end; ++i) {
        if (ischar) {
            pylst += "'";
            pylst += (char)i;
            pylst += "'";
        }
        else
            pylst += std::to_string(i);
        if (i != end - 1)
            pylst += ", ";
    }

    if (expr->m_inclusive) {
        if (ischar) {
            pylst += ", '";
            pylst += (char)end;
            pylst += "'";
        }
        else
            pylst += ", " + std::to_string(end);
    }

    pylst += "]";
    return pylst;
}

static std::string
expr_term_tuple_to_py(ExprTuple *expr, Context &ctx) {
    std::string pyexprs = "";
    for (size_t i = 0; i < expr->m_exprs.size(); ++i) {
        pyexprs += expr_to_py(expr->m_exprs.at(i).get(), ctx);
        if (i != expr->m_exprs.size()-1)
            pyexprs += ", ";
    }
    return "("+pyexprs+")";
}

static std::string
expr_term_slice_to_py(ExprSlice *expr, Context &ctx) {
    std::string pystart = "", pyend = "";
    if (expr->m_start.has_value())
        pystart = expr_to_py(expr->m_start.value().get(), ctx);
    if (expr->m_end.has_value())
        pyend = expr_to_py(expr->m_end.value().get(), ctx);
    return pystart+":"+pyend;
}

static std::string
expr_term_dict_to_py(ExprDict *expr, Context &ctx) {
    std::string pydict = "{";

    for (size_t i = 0; i < expr->m_values.size(); ++i) {
        pydict += expr_to_py(expr->m_values.at(i).first.get(), ctx);
        pydict += ": ";
        pydict += expr_to_py(expr->m_values.at(i).second.get(), ctx);
        if (i != expr->m_values.size() - 1)
            pydict += ", ";
    }

    pydict += "}";
    return pydict;
}

static std::string
expr_term_fstr_to_py(ExprFStr *expr, Context &ctx) {
    return "f\""+expr->m_tok->lexeme()+"\"";
}

static std::string
expr_term_to_py(ExprTerm *expr, Context &ctx) {
    switch (expr->get_term_type()) {
    case ExprTermType::Ident:         return expr_term_ident_to_py(dynamic_cast<ExprIdent *>(expr), ctx);
    case ExprTermType::Int_Literal:   return expr_term_intlit_to_py(dynamic_cast<ExprIntLit *>(expr), ctx);
    case ExprTermType::Str_Literal:   return expr_term_strlit_to_py(dynamic_cast<ExprStrLit *>(expr), ctx);
    case ExprTermType::Char_Literal:  return expr_term_charlit_to_py(dynamic_cast<ExprCharLit *>(expr), ctx);
    case ExprTermType::Float_Literal: return expr_term_floatlit_to_py(dynamic_cast<ExprFloatLit *>(expr), ctx);
    case ExprTermType::Func_Call:     return expr_term_funccall_to_py(dynamic_cast<ExprFuncCall *>(expr), ctx);
    case ExprTermType::List_Literal:  return expr_term_listlit_to_py(dynamic_cast<ExprListLit *>(expr), ctx);
    case ExprTermType::Get:           return expr_term_get_to_py(dynamic_cast<ExprGet *>(expr), ctx);
    case ExprTermType::Mod_Access:    return expr_term_mod_to_py_access(dynamic_cast<ExprModAccess *>(expr), ctx);
    case ExprTermType::Array_Access:  return expr_term_array_access_to_py(dynamic_cast<ExprArrayAccess *>(expr), ctx);
    case ExprTermType::Bool:          return expr_term_boollit_to_py(dynamic_cast<ExprBool *>(expr), ctx);
    case ExprTermType::None:          return expr_term_none_to_py(dynamic_cast<ExprNone *>(expr), ctx);
    case ExprTermType::Closure:       return expr_term_closure_to_py(dynamic_cast<ExprClosure *>(expr), ctx);
    case ExprTermType::Range:         return expr_term_range_to_py(dynamic_cast<ExprRange *>(expr), ctx);
    case ExprTermType::Tuple:         return expr_term_tuple_to_py(dynamic_cast<ExprTuple *>(expr), ctx);
    case ExprTermType::Slice:         return expr_term_slice_to_py(dynamic_cast<ExprSlice *>(expr), ctx);
    case ExprTermType::Dict:          return expr_term_dict_to_py(dynamic_cast<ExprDict *>(expr), ctx);
    case ExprTermType::FStr:          return expr_term_fstr_to_py(dynamic_cast<ExprFStr *>(expr), ctx);
    default: {
        std::string msg = "unknown term: `"+std::to_string((int)expr->get_term_type())+"`";
        throw InterpreterException(msg);
    }
    }
}

static std::string
expr_bin_to_py(ExprBinary *expr, Context &ctx) {
    std::string lhs = expr_to_py(expr->m_lhs.get(), ctx);
    std::string rhs = expr_to_py(expr->m_rhs.get(), ctx);
    return lhs+" "+expr->m_op->lexeme()+" "+rhs;
}

static std::string
expr_unary_to_py(ExprUnary *expr, Context &ctx) {
    std::string pyexpr = expr_to_py(expr->m_expr.get(), ctx);
    return expr->m_op->lexeme()+pyexpr;
}

static std::string
expr_to_py(Expr *expr, Context &ctx) {
    switch (expr->get_type()) {
    case ExprType::Term: {
        return expr_term_to_py(dynamic_cast<ExprTerm *>(expr), ctx);
    } break;
    case ExprType::Binary: {
        return expr_bin_to_py(dynamic_cast<ExprBinary *>(expr), ctx);
    } break;
    case ExprType::Unary: {
        return expr_unary_to_py(dynamic_cast<ExprUnary *>(expr), ctx);
    } break;
    default: assert(false && "unreachable"); break;
    }
}

static void
stmt_def_to_py(StmtDef *stmt, Context &ctx, bool add_self = false) {
    const std::string &id = stmt->m_id->lexeme();
    std::string params = "";

    if (add_self)
        params += "self";

    for (size_t i = 0; i < stmt->m_args.size(); ++i) {
        params += stmt->m_args.at(i).first.first->lexeme();
        if (i != stmt->m_args.size() - 1)
            params += ", ";
    }
    PYSTMT_CONS("def "+id+"("+params+"):", ctx);
    stmt_block_to_py(stmt->m_block.get(), ctx);
}

static void
stmt_let_to_py(StmtLet *stmt, Context &ctx, bool __init__ = false) {
    std::string pylet = "";
    for (size_t i = 0; i < stmt->m_ids.size(); ++i) {
        std::string id = "";
        if (__init__)
            pylet += "self.";
        pylet += stmt->m_ids.at(i)->lexeme();
        if (i != stmt->m_ids.size()-1)
            pylet += ", ";
    }

    // TODO: remove if found unnecessary
    // if (stmt->m_expr->get_type() == ExprType::Term) {
    //     auto term = dynamic_cast<ExprTerm *>(stmt->m_expr.get());
    //     if (term->get_term_type() == ExprTermType::Closure) {
    //         auto cl = dynamic_cast<ExprClosure *>(term);
    //         std::string clname = construct_closure(cl, ctx);
    //         PYSTMT_CONS(pylet+" = "+clname, ctx);
    //         return;
    //     }
    // }

    const std::string pyexpr = expr_to_py(stmt->m_expr.get(), ctx);
    PYSTMT_CONS(pylet+" = " + pyexpr, ctx);
}

static void
stmt_block_to_py(StmtBlock *stmt, Context &ctx) {
    ctx.scope_depth++;

    if (stmt->m_stmts.size() == 0)
        PYSTMT("pass", ctx);

    for (size_t i = 0; i < stmt->m_stmts.size(); ++i)
        stmt_to_py(stmt->m_stmts.at(i).get(), ctx);

    ctx.scope_depth--;
}

static void
stmt_mut_to_py(StmtMut *stmt, Context &ctx) {
    const std::string pyleft = expr_to_py(stmt->m_left.get(), ctx);
    const std::string pyright = expr_to_py(stmt->m_right.get(), ctx);
    PYSTMT_CONS(pyleft+" "+stmt->m_equals->lexeme()+" "+pyright, ctx);
}

static void
stmt_expr_to_py(StmtExpr *stmt, Context &ctx) {
    const std::string pyexpr = expr_to_py(stmt->m_expr.get(), ctx);
    PYSTMT_CONS(pyexpr, ctx);
}

static void
stmt_if_to_py(StmtIf *stmt, Context &ctx) {
    const std::string pyexpr = expr_to_py(stmt->m_expr.get(), ctx);
    PYSTMT_CONS("if "+pyexpr+":", ctx);
    stmt_block_to_py(stmt->m_block.get(), ctx);
    if (stmt->m_else.has_value()) {
        PYSTMT("else:", ctx);
        stmt_block_to_py(stmt->m_else.value().get(), ctx);
    }
}

static void
stmt_return_to_py(StmtReturn *stmt, Context &ctx) {
    std::string pyexpr = "";
    if (stmt->m_expr.has_value())
        pyexpr += " " + expr_to_py(stmt->m_expr.value().get(), ctx);

    PYSTMT_CONS("return"+pyexpr, ctx);
}

static void
stmt_break_to_py(StmtBreak *stmt, Context &ctx) {
    PYSTMT("break", ctx);
}

static void
stmt_while_to_py(StmtWhile *stmt, Context &ctx) {
    const std::string pyexpr = expr_to_py(stmt->m_expr.get(), ctx);
    PYSTMT_CONS("while "+pyexpr+":", ctx);
    stmt_block_to_py(stmt->m_block.get(), ctx);
}

static void
stmt_foreach_to_py(StmtForeach *stmt, Context &ctx) {
    std::string pyloop = "for ";
    for (size_t i = 0; i < stmt->m_enumerators.size(); ++i) {
        pyloop += stmt->m_enumerators.at(i)->lexeme();
        if (i != stmt->m_enumerators.size()-1)
            pyloop += ", ";
    }
    pyloop += " in " + expr_to_py(stmt->m_expr.get(), ctx) + ":";
    PYSTMT_CONS(pyloop, ctx);
    stmt_block_to_py(stmt->m_block.get(), ctx);
}

static void
stmt_for_to_py(StmtFor *stmt, Context &ctx) {
    std::string enumerator = stmt->m_enumerator->lexeme();
    std::string start = expr_to_py(stmt->m_start.get(), ctx);
    std::string end = expr_to_py(stmt->m_end.get(), ctx);
    std::string range = "range("+start+", "+end+")";
    PYSTMT_CONS("for " + enumerator + " in " + range + ":", ctx);
    stmt_block_to_py(stmt->m_block.get(), ctx);
}

static void
stmt_import_to_py(StmtImport *stmt, Context &ctx) {
    const std::string &pyfp = stmt->m_fp->lexeme();
    std::string alias = "";
    if (stmt->m_as.has_value())
        alias = stmt->m_as.value()->lexeme();

    std::string pyimport = "# IMPORT NEEDS RESOLVING\n# import " + pyfp;
    if (alias != "")
        pyimport += " as " + alias;

    ctx.earl_imports.push_back(pyimport);

    // PYSTMT_CONS(pyimport, ctx);
}

static void
stmt_mod_to_py(StmtMod *stmt, Context &ctx) {
    (void)stmt;
    (void)ctx;
    // nothing to do...
}

static void
stmt_class_to_py(StmtClass *stmt, Context &ctx) {
    std::string pyclass = "class " + stmt->m_id->lexeme() + ":";
    PYSTMT_CONS(pyclass, ctx);

    ctx.in_class = true;
    ctx.scope_depth++;

    if (stmt->m_constructor_args.size() > 0) {
        std::string init = "def __init__(self, ";
        for (size_t i = 0; i < stmt->m_constructor_args.size(); ++i) {
            init += stmt->m_constructor_args.at(i).first->lexeme();
            if (i != stmt->m_constructor_args.size()-1)
                init += ", ";
        }
        init += "):";
        PYSTMT_CONS(init, ctx);
        ctx.scope_depth++;
        for (size_t i = 0; i < stmt->m_members.size(); ++i)
            stmt_let_to_py(stmt->m_members.at(i).get(), ctx, true);
        ctx.scope_depth--;
    }

    for (size_t i = 0; i < stmt->m_methods.size(); ++i)
        stmt_def_to_py(stmt->m_methods.at(i).get(), ctx, /*add_self=*/true);

    ctx.in_class = false;
    ctx.scope_depth--;
}

static void
stmt_match_to_py(StmtMatch *stmt, Context &ctx) {
}

static void
stmt_enum_to_py(StmtEnum *stmt, Context &ctx) {
}

static void
stmt_continue_to_py(StmtContinue *stmt, Context &ctx) {
    PYSTMT("continue", ctx);
}

static void
stmt_loop_to_py(StmtLoop *stmt, Context &ctx) {
    PYSTMT("while True:", ctx);
    stmt_block_to_py(stmt->m_block.get(), ctx);
}

static void
stmt_to_py(Stmt *stmt, Context &ctx) {
    switch (stmt->stmt_type()) {
    case StmtType::Def:      stmt_def_to_py(dynamic_cast<StmtDef *>(stmt), ctx); break;
    case StmtType::Let:      stmt_let_to_py(dynamic_cast<StmtLet *>(stmt), ctx); break;
    case StmtType::Block:    stmt_block_to_py(dynamic_cast<StmtBlock *>(stmt), ctx); break;
    case StmtType::Mut:      stmt_mut_to_py(dynamic_cast<StmtMut *>(stmt), ctx); break;
    case StmtType::Stmt_Expr:stmt_expr_to_py(dynamic_cast<StmtExpr *>(stmt), ctx); break;
    case StmtType::If:       stmt_if_to_py(dynamic_cast<StmtIf *>(stmt), ctx); break;
    case StmtType::Return:   stmt_return_to_py(dynamic_cast<StmtReturn *>(stmt), ctx); break;
    case StmtType::Break:    stmt_break_to_py(dynamic_cast<StmtBreak *>(stmt), ctx); break;
    case StmtType::While:    stmt_while_to_py(dynamic_cast<StmtWhile *>(stmt), ctx); break;
    case StmtType::Foreach:  stmt_foreach_to_py(dynamic_cast<StmtForeach *>(stmt), ctx); break;
    case StmtType::For:      stmt_for_to_py(dynamic_cast<StmtFor *>(stmt), ctx); break;
    case StmtType::Import:   stmt_import_to_py(dynamic_cast<StmtImport *>(stmt), ctx); break;
    case StmtType::Mod:      stmt_mod_to_py(dynamic_cast<StmtMod *>(stmt), ctx); break;
    case StmtType::Class:    stmt_class_to_py(dynamic_cast<StmtClass *>(stmt), ctx); break;
    case StmtType::Match:    stmt_match_to_py(dynamic_cast<StmtMatch *>(stmt), ctx); break;
    case StmtType::Enum:     stmt_enum_to_py(dynamic_cast<StmtEnum *>(stmt), ctx); break;
    case StmtType::Continue: stmt_continue_to_py(dynamic_cast<StmtContinue *>(stmt), ctx); break;
    case StmtType::Loop:     stmt_loop_to_py(dynamic_cast<StmtLoop *>(stmt), ctx); break;
    default: assert(false && "unreachable");
    }
}

std::string
earl_to_py(std::unique_ptr<Program> program) {
    Context ctx = {0, "", false, {}, {}, {}};
    for (size_t i = 0; i < program->m_stmts.size(); ++i) {
        stmt_to_py(program->m_stmts.at(i).get(), ctx);
    }

    std::string imports = "";
    for (auto &l : ctx.earl_imports)
        imports += l + "\n";
    imports += "\n";

    std::string py_imports = "";
    for (auto &l : ctx.py_imports)
        py_imports += "import "+l+"\n";
    py_imports += "\n";

    std::string lambdas = "";
    for (auto &l : ctx.lambdas)
        lambdas += l + "\n";

    std::string py_intrinsics = "";
    for (auto &l : ctx.py_member_intrinsics)
        py_intrinsics += l.second + "\n";

    return py_imports+imports+lambdas+py_intrinsics+ctx.py_src;
}

