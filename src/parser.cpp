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

#include <cassert>
#include <iostream>
#include <optional>
#include <memory>
#include <optional>

#include "utils.hpp"
#include "err.hpp"
#include "ast.hpp"
#include "common.hpp"
#include "parser.hpp"

std::vector<std::pair<std::pair<std::shared_ptr<Token>, std::optional<std::shared_ptr<__Type>>>, uint32_t>>
parse_stmt_def_args(Lexer &lexer);

static Attr
translate_attr(Lexer &lexer) {
    auto errtok = Parser::parse_expect(lexer, TokenType::At);

    std::shared_ptr<Token> attr = Parser::parse_expect(lexer, TokenType::Ident);
    if (attr->lexeme() == COMMON_EARLATTR_PUB)
        return Attr::Pub;
    if (attr->lexeme() == COMMON_EARLATTR_WORLD)
        return Attr::World;
    if (attr->lexeme() == COMMON_EARLATTR_REF)
        return Attr::Ref;
    if (attr->lexeme() == COMMON_EARLATTR_CONST)
        return Attr::Const;
    if (attr->lexeme() == COMMON_EARLATTR_EXPERIMENTAL)
        return Attr::Experimental;
    else {
        Err::err_wtok(errtok.get());
        std::string msg = "unknown attribute `" + attr->lexeme() + "`";
        throw ParserException(msg);
    }
}

static uint32_t
gather_attrs(Lexer &lexer) {
    uint32_t attrs = 0;
    while (lexer.peek(0) && lexer.peek(0)->type() == TokenType::At)
        attrs |= static_cast<uint32_t>(translate_attr(lexer));
    return attrs;
}

static bool
is_ty(Lexer &lexer) {
    if (lexer.peek(0) && lexer.peek(0)->type() == TokenType::Colon
        && lexer.peek(1) && lexer.peek(1)->type() == TokenType::Ident)
        return true;
    return false;
}

static std::shared_ptr<__Type>
get_ty(Lexer &lexer) {
    std::shared_ptr<Token> main_ty = nullptr;
    std::optional<std::shared_ptr<Token>> sub_ty = {};

    lexer.discard(); // :
    main_ty = lexer.next();

    if (lexer.peek(0) && lexer.peek(0)->type() == TokenType::Double_Colon) {
        lexer.discard();
        main_ty = Parser::parse_expect(lexer, TokenType::Ident);
    }

    return std::make_shared<__Type>(main_ty, sub_ty);
}

std::shared_ptr<Token>
Parser::parse_expect(Lexer &lexer, TokenType expected) {
    std::shared_ptr<Token> tok = lexer.next();
    if (tok->type() != expected) {
        Err::err_wtok(tok.get());
        std::string msg = "expected "
            + tokentype_to_str(expected)
            + ", got "
            + tokentype_to_str(tok->m_type)
            + " `" + tok->lexeme() + "`";
        throw ParserException(msg);
    }
    return tok;
}

std::shared_ptr<Token>
Parser::parse_expect_keyword(Lexer &lexer, std::string expected) {
    std::shared_ptr<Token> tok = lexer.next();
    if (tok->type() != TokenType::Keyword) {
        Err::err_wtok(tok.get());
        std::string msg = "expected keyword `"+expected+"`, but got `" + tok->lexeme() + "` which is not a keyword";
        throw ParserException(msg);
    }
    if (tok->lexeme() != expected) {
        Err::err_wtok(tok.get());
        std::string msg = "expected keyword `"+expected+"`, but got `" + tok->lexeme() + "` which is not the correct keyword";
        throw ParserException(msg);
    }
    return tok;
}

static std::vector<std::shared_ptr<Token>>
parse_comma_sep_idents_as_toks(Lexer &lexer) {
    std::vector<std::shared_ptr<Token>> ids = {};

    while (lexer.peek(0) && lexer.peek(0)->type() == TokenType::Ident) {
        ids.push_back(lexer.next());
        if (lexer.peek(0) && lexer.peek(0)->type() == TokenType::Comma) {
            lexer.discard(); // ,
        }
        else {
            if (lexer.peek(0) && lexer.peek(0)->type() == TokenType::Ident) {
                Err::err_wtok(lexer.peek(0));
                const std::string msg = "missing comma after identifer";
                throw ParserException(msg);
            }
        }
    }

    return ids;
}

// NOTE: It is up to the calling function to consume the '()' or '[]' or '{}' etc.
// This makes this function more reusable.
static std::vector<Expr *>
parse_comma_sep_exprs(Lexer &lexer, bool &trailing_comma) {
    std::vector<Expr *> exprs;

    while (1) {
        // Only needed if no arguments are provided.
        if (lexer.peek(0) && lexer.peek()->type() == TokenType::Rparen
            || lexer.peek()->type() == TokenType::Rbrace
            || lexer.peek()->type() == TokenType::Rbracket) {
            break;
        }
        exprs.push_back(Parser::parse_expr(lexer));
        if (lexer.peek(0) && lexer.peek()->type() == TokenType::Comma) {
            (void)Parser::parse_expect(lexer, TokenType::Comma);
        if (lexer.peek(0) && lexer.peek()->type() == TokenType::Rparen
            || lexer.peek()->type() == TokenType::Rbrace
            || lexer.peek()->type() == TokenType::Rbracket)
            trailing_comma = true;
        }
        else
            break;
    }

    return exprs;
}

static std::optional<std::vector<std::unique_ptr<Expr>>>
try_parse_funccall(Lexer &lexer) {
    if (lexer.peek(0) && lexer.peek()->type() == TokenType::Lparen) {
        (void)Parser::parse_expect(lexer, TokenType::Lparen);
        bool unused = false;
        std::vector<Expr *> exprs = parse_comma_sep_exprs(lexer, unused);
        std::vector<std::unique_ptr<Expr>> unique_exprs = {};
        for (size_t i = 0; i < exprs.size(); ++i)
            unique_exprs.push_back(std::unique_ptr<Expr>(exprs[i]));
        (void)Parser::parse_expect(lexer, TokenType::Rparen);
        return unique_exprs;
    }

    return {};
}

static std::variant<std::unique_ptr<ExprIdent>, std::unique_ptr<ExprFuncCall>>
parse_identifier_or_funccall(Lexer &lexer) {
    auto ident = std::make_unique<ExprIdent>(lexer.next());
    auto group = try_parse_funccall(lexer);

    if (group.has_value())
        return std::make_unique<ExprFuncCall>(std::move(ident), std::move(group.value()), ident->m_tok);

    return ident;
}

static std::vector<std::pair<std::shared_ptr<Token>, uint32_t>>
parse_closure_args(Lexer &lexer) {
    std::vector<std::pair<std::shared_ptr<Token>, uint32_t>> args;

    while (lexer.peek(0) && lexer.peek()->type() != TokenType::Pipe) {
        uint32_t attr = 0;

        while (lexer.peek(0) && lexer.peek()->type() == TokenType::At) {
            attr |= static_cast<uint32_t>(translate_attr(lexer));
        }

        std::shared_ptr<Token> id = Parser::parse_expect(lexer, TokenType::Ident);
        std::shared_ptr<Token> var = std::move(id);
        args.push_back(std::make_pair(std::move(var), attr));

        if (lexer.peek(0) && lexer.peek()->type() == TokenType::Comma)
            lexer.discard();
    }
    (void)Parser::parse_expect(lexer, TokenType::Pipe);

    return args;
}

static std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<Expr>>>
parse_set_values(Lexer &lexer) {
    std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<Expr>>> values = {};
    while (lexer.peek(0) && lexer.peek(0)->type() != TokenType::Rbrace) {
        Expr *key = Parser::parse_expr(lexer, /*fail_on=*/':');
        Parser::parse_expect(lexer, TokenType::Colon);
        Expr *value = Parser::parse_expr(lexer);
        values.push_back(std::make_pair(std::unique_ptr<Expr>(key), std::unique_ptr<Expr>(value)));
        if (lexer.peek(0) && lexer.peek(0)->type() == TokenType::Comma)
            lexer.discard(); // ,
        else {
            Parser::parse_expect(lexer, TokenType::Rbrace);
            break;
        }
    }
    return values;
}

static Expr *
parse_primary_expr(Lexer &lexer, char fail_on = '\0') {
    Token *tok = nullptr;
    Expr *left = nullptr;
    Expr *right = nullptr;

    // Unary expressions
    while (lexer.peek(0) && (lexer.peek()->type() == TokenType::Minus
                             || lexer.peek()->type() == TokenType::Bang
                             || lexer.peek()->type() == TokenType::Backtick_Tilde)) {
        std::shared_ptr<Token> op = lexer.next();
        Expr *operand = parse_primary_expr(lexer, fail_on);
        left = new ExprUnary(std::move(op), std::unique_ptr<Expr>(operand));
    }

    while (1) {
        if (!lexer.peek(0))
            return left;
        switch (lexer.peek()->type()) {
        case TokenType::Ident: {
            left = new ExprIdent(lexer.next());
        } break;
        case TokenType::Lbrace: {
            if (fail_on == '{')
                return left;
            auto tok = lexer.next();
            auto values = parse_set_values(lexer);
            left = new ExprDict(std::move(values), tok);
        } break;
        case TokenType::Lparen: {
            auto tok = lexer.next(); // (
            bool trailing_comma = false;
            std::vector<Expr *> tuple = parse_comma_sep_exprs(lexer, trailing_comma);
            (void)Parser::parse_expect(lexer, TokenType::Rparen);
            // Function call
            if (left) {
                std::vector<std::unique_ptr<Expr>> unique_tuple = {};
                for (size_t i = 0; i < tuple.size(); ++i)
                    unique_tuple.push_back(std::unique_ptr<Expr>(tuple[i]));
                left = new ExprFuncCall(std::unique_ptr<Expr>(left), std::move(unique_tuple), tok);
            }
            // Tuple
            else if (tuple.size() > 1 || trailing_comma) {
                std::vector<std::unique_ptr<Expr>> unique_tuple = {};
                for (size_t i = 0; i < tuple.size(); ++i)
                    unique_tuple.push_back(std::unique_ptr<Expr>(tuple[i]));
                left = new ExprTuple(std::move(unique_tuple), tok);
            }
            // Math
            else
                left = std::move(tuple[0]);
        } break;
        case TokenType::Period: {
            if (!left) {
                Err::err_wtok(lexer.peek(0));
                const std::string msg = "cannot use dot notation `.` with no lhs";
                throw ParserException(msg);
            }
            auto tok = lexer.next();
            left = new ExprGet(std::unique_ptr<Expr>(left), parse_identifier_or_funccall(lexer), tok);
        } break;
        case TokenType::Intlit: {
            left = new ExprIntLit(lexer.next());
        } break;
        case TokenType::Floatlit: {
            left = new ExprFloatLit(lexer.next());
        } break;
        case TokenType::Strlit: {
            if (left && left->get_type() == ExprType::Term) {
                auto left_term = dynamic_cast<ExprTerm *>(left);
                if (left_term->get_term_type() == ExprTermType::Ident) {
                    auto left_ident = dynamic_cast<ExprIdent *>(left_term);
                    if (left_ident->m_tok->lexeme() == "f")
                        left = new ExprFStr(lexer.next());
                    else
                        goto not_fstr;
                }
                else
                    goto not_fstr;
            }
            else {
            not_fstr:
                left = new ExprStrLit(lexer.next());
            }
        } break;
        case TokenType::Charlit: {
            left = new ExprCharLit(lexer.next());
        } break;
        case TokenType::Lbracket: {
            if (left) {
                auto tok = lexer.next(); // [
                Expr *idx = Parser::parse_expr(lexer);
                if (!idx) {
                    Err::err_wtok(tok.get());
                    const std::string msg = "list access requires an index";
                    throw ParserException(msg);
                }
                (void)Parser::parse_expect(lexer, TokenType::Rbracket);
                left = new ExprArrayAccess(std::unique_ptr<Expr>(left), std::unique_ptr<Expr>(idx), tok);
            }
            else {
                auto tok = lexer.next(); // [
                bool unused = false;
                std::vector<Expr *> lst = parse_comma_sep_exprs(lexer, unused);
                std::vector<std::unique_ptr<Expr>> unique_lst = {};
                for (size_t i = 0; i < lst.size(); ++i)
                    unique_lst.push_back(std::unique_ptr<Expr>(lst[i]));
                (void)Parser::parse_expect(lexer, TokenType::Rbracket);
                left = new ExprListLit(std::move(unique_lst), tok);
            }
        } break;
        case TokenType::Double_Colon: {
            auto tok = lexer.next(); // ::

            if (left->get_type() == ExprType::Term) {
                auto _left = dynamic_cast<ExprTerm *>(left);
                if (_left->get_term_type() == ExprTermType::Ident)
                    left = new ExprModAccess(std::unique_ptr<ExprIdent>(dynamic_cast<ExprIdent *>(_left)),
                                             parse_identifier_or_funccall(lexer),
                                             tok);
                else {
                    std::string msg = "Module getters must be of term type identifier";
                    throw ParserException(msg);
                }
            }
            else {
                std::string msg = "Module getters must be of term type identifier";
                throw ParserException(msg);
            }

        } break;
        case TokenType::Pipe: {
            // Workaround for when parsing a `match` statement since
            // the individual branches can have `|` in arm. We want to
            // fail here and let the callees handle it.
            if (fail_on == '|')
                return left;
            auto tok = lexer.next(); // |
            std::vector<std::pair<std::shared_ptr<Token>, uint32_t>> args = parse_closure_args(lexer);
            auto block = Parser::parse_stmt_block(lexer);
            return new ExprClosure(std::move(args), std::move(block), tok);
        }
        case TokenType::Keyword: {
            if (lexer.peek(0) && lexer.peek()->lexeme() == COMMON_EARLKW_TO)
                return left;
            if (lexer.peek(0) && lexer.peek()->lexeme() == COMMON_EARLKW_WHEN)
                return left;

            std::shared_ptr<Token> kw = lexer.next();
            if (kw->lexeme() == COMMON_EARLKW_TRUE) {
                return new ExprBool(std::move(kw), true);
            }
            else if (kw->lexeme() == COMMON_EARLKW_FALSE) {
                return new ExprBool(std::move(kw), false);
            }
            else if (kw->lexeme() == COMMON_EARLKW_NONE) {
                return new ExprNone(std::move(kw));
            }
            else {
                Err::err_wtok(kw.get());
                std::string msg = "invalid keyword `" + kw->lexeme() + "` while parsing primary expression";
                throw ParserException(msg);
            }
        } break;
        default: {
            return left;
        } break;
        }
    }
    return left;
}

static Expr *
parse_power_expr(Lexer &lexer, char fail_on='\0') {
    Expr *lhs = parse_primary_expr(lexer, fail_on);
    Token *cur = lexer.peek();
    while (cur && (cur->type() == TokenType::Double_Asterisk)) {
        std::shared_ptr<Token> op = lexer.next();
        Expr *rhs = parse_primary_expr(lexer, fail_on);
        lhs = new ExprBinary(std::unique_ptr<Expr>(lhs),
                             std::move(op),
                             std::unique_ptr<Expr>(rhs));
        cur = lexer.peek();
    }
    return lhs;
}

static Expr *
parse_multiplicative_expr(Lexer &lexer, char fail_on = '\0') {
    Expr *lhs = parse_power_expr(lexer, fail_on);
    Token *cur = lexer.peek();
    while (cur && (cur->type() == TokenType::Asterisk
                   || cur->type() == TokenType::Forwardslash
                   || cur->type() == TokenType::Percent)) {
        std::shared_ptr<Token> op = lexer.next();
        Expr *rhs = parse_power_expr(lexer, fail_on);
        lhs = new ExprBinary(std::unique_ptr<Expr>(lhs),
                             std::move(op),
                             std::unique_ptr<Expr>(rhs));
        cur = lexer.peek();
    }
    return lhs;
}

static Expr *
parse_additive_expr(Lexer &lexer, char fail_on = '\0') {
    Expr *lhs = parse_multiplicative_expr(lexer, fail_on);
    Token *cur = lexer.peek();
    while (cur && (cur->type() == TokenType::Plus
                   || cur->type() == TokenType::Minus)) {
        std::shared_ptr<Token> op = lexer.next();
        Expr *rhs = parse_multiplicative_expr(lexer, fail_on);
        lhs = new ExprBinary(std::unique_ptr<Expr>(lhs),
                             std::move(op),
                             std::unique_ptr<Expr>(rhs));
        cur = lexer.peek();
    }
    return lhs;
}

static Expr *
parse_equalitative_expr(Lexer &lexer, char fail_on = '\0') {
    Expr *lhs = parse_additive_expr(lexer, fail_on);
    Token *cur = lexer.peek();
    while (cur && (cur->type() == TokenType::Double_Equals
                   || cur->type() == TokenType::Greaterthan_Equals
                   || cur->type() == TokenType::Greaterthan
                   || cur->type() == TokenType::Lessthan_Equals
                   || cur->type() == TokenType::Lessthan
                   || cur->type() == TokenType::Bang_Equals)) {
        std::shared_ptr<Token> op = lexer.next();
        Expr *rhs = parse_additive_expr(lexer, fail_on);
        lhs = new ExprBinary(std::unique_ptr<Expr>(lhs),
                             std::move(op),
                             std::unique_ptr<Expr>(rhs));
        cur = lexer.peek();
    }
    return lhs;
}

static Expr *
parse_logical_expr(Lexer &lexer, char fail_on = '\0') {
    Expr *lhs = parse_equalitative_expr(lexer, fail_on);
    Token *cur = lexer.peek();
    while (cur && (cur->type() == TokenType::Double_Ampersand
                   || cur->type() == TokenType::Double_Pipe)) {
        std::shared_ptr<Token> op = lexer.next();
        Expr *rhs = parse_equalitative_expr(lexer, fail_on);
        lhs = new ExprBinary(std::unique_ptr<Expr>(lhs),
                             std::move(op),
                             std::unique_ptr<Expr>(rhs));
        cur = lexer.peek();
    }
    return lhs;
}

static Expr *
parse_bitwise_expr(Lexer &lexer, char fail_on = '\0') {
    Expr *lhs = parse_logical_expr(lexer, fail_on);
    Token *cur = lexer.peek();
    while (cur && (cur->type() == TokenType::Double_Greaterthan
                   || cur->type() == TokenType::Double_Lessthan
                   || cur->type() == TokenType::Backtick_Caret
                   || cur->type() == TokenType::Backtick_Pipe
                   || cur->type() == TokenType::Backtick_Ampersand)) {
        std::shared_ptr<Token> op = lexer.next();
        Expr *rhs = parse_logical_expr(lexer);
        lhs = new ExprBinary(std::unique_ptr<Expr>(lhs),
                             std::move(op),
                             std::unique_ptr<Expr>(rhs));
        cur = lexer.peek();
    }
    return lhs;
}

static Expr *
parse_slice_expr(Lexer &lexer, char fail_on = '\0') {
    Expr *lhs = parse_bitwise_expr(lexer, fail_on);
    Token *cur = lexer.peek();
    while (fail_on != ':' && cur && (cur->type() == TokenType::Colon)) {
        auto tok = lexer.next(); // :
        Expr *rhs = Parser::parse_expr(lexer, fail_on);
        std::optional<std::unique_ptr<Expr>> l = {}, r = {};
        if (lhs)
            l = std::unique_ptr<Expr>(lhs);
        if (rhs)
            r = std::unique_ptr<Expr>(rhs);
        lhs = new ExprSlice(std::move(l), std::move(r), tok);
        cur = lexer.peek();
    }
    return lhs;
}

static Expr *
parse_range_expr(Lexer &lexer, char fail_on = '\0') {
    Expr *lhs = parse_slice_expr(lexer, fail_on);
    Token *cur = lexer.peek();
    while (cur && (cur->type() == TokenType::Double_Period)) {
        auto tok = lexer.next(); // ..
        Expr *rhs = nullptr;
        if (lexer.peek(0) && lexer.peek(0)->type() == TokenType::Equals) {
            lexer.discard(); // =
            rhs = Parser::parse_expr(lexer, /*fail_on=*/'{');
            lhs = new ExprRange(std::unique_ptr<Expr>(lhs), std::unique_ptr<Expr>(rhs), true, tok);
        }
        else {
            rhs = Parser::parse_expr(lexer, /*fail_on=*/'{');
            if (!rhs) {
                Err::err_wtok(lexer.peek(0));
                std::string msg = "cannot use a range where the end expression is empty";
                throw ParserException(msg);
            }
            lhs = new ExprRange(std::unique_ptr<Expr>(lhs), std::unique_ptr<Expr>(rhs), false, tok);
        }

        cur = lexer.peek();
    }
    return lhs;
}

Expr *
Parser::parse_expr(Lexer &lexer, char fail_on) {
    return parse_range_expr(lexer, fail_on);
}

std::unique_ptr<StmtMut>
Parser::parse_stmt_mut(Lexer &lexer) {
    Expr *left = Parser::parse_expr(lexer);
    auto tok = lexer.next(); // =, +=, -=, etc
    Expr *right = Parser::parse_expr(lexer);
    (void)parse_expect(lexer, TokenType::Semicolon);
    return std::make_unique<StmtMut>(std::unique_ptr<Expr>(left),
                                     std::unique_ptr<Expr>(right),
                                     std::move(tok));
}

std::unique_ptr<StmtIf>
Parser::parse_stmt_if(Lexer &lexer) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_IF);

    Expr *expr = Parser::parse_expr(lexer, /*fail_on=*/'{');
    std::unique_ptr<StmtBlock> block = Parser::parse_stmt_block(lexer);

    // Handle the `else if` or `else` blocks if applicable
    std::optional<std::unique_ptr<StmtBlock>> else_ = {};
    Token *tok1 = lexer.peek();
    Token *tok2 = lexer.peek(1);

    bool tok1_else = tok1->type() == TokenType::Keyword && tok1->lexeme() == COMMON_EARLKW_ELSE;
    bool tok2_if = tok2 && (tok2->type() == TokenType::Keyword && tok2->lexeme() == COMMON_EARLKW_IF);

    if (tok1_else && tok2_if) {
        lexer.discard();
        std::vector<std::unique_ptr<Stmt>> tmp;
        std::unique_ptr<StmtIf> nested_if = parse_stmt_if(lexer);
        tmp.push_back(std::move(nested_if));
        else_ = std::make_unique<StmtBlock>(std::move(tmp));
    }
    else if (tok1_else) {
        Parser::parse_expect_keyword(lexer, COMMON_EARLKW_ELSE);
        else_ = parse_stmt_block(lexer);
    }

    return std::make_unique<StmtIf>(std::unique_ptr<Expr>(expr),
                                    std::move(block),
                                    std::move(else_));
}

std::unique_ptr<StmtLet>
Parser::parse_stmt_let(Lexer &lexer, uint32_t attrs, std::vector<std::string> info) {
    auto errtok = Parser::parse_expect_keyword(lexer, COMMON_EARLKW_LET);

    std::vector<std::shared_ptr<Token>> ids = {parse_expect(lexer, TokenType::Ident)};
    std::vector<std::shared_ptr<__Type>> tys = {};

    if (is_ty(lexer)) {
        tys.push_back(get_ty(lexer));
    }

    while (lexer.peek(0) && lexer.peek(0)->type() == TokenType::Comma) {
        lexer.discard(); // ,
        ids.push_back(parse_expect(lexer, TokenType::Ident));
        if (is_ty(lexer))
            tys.push_back(get_ty(lexer));
    }

    if (tys.size() > 0 && ids.size() != tys.size()) {
        Err::err_wtok(errtok.get());
        const std::string msg = "when declaring multiple variables with explicit types, all variables must have a type";
        throw ParserException(msg);
    }

    (void)parse_expect(lexer, TokenType::Equals);
    Expr *expr = Parser::parse_expr(lexer);

    if (!expr) {
        Err::err_wtok(lexer.peek());
        std::string msg = "invalid token `" + lexer.peek()->lexeme() + "` while parsing primary expression";
        throw ParserException(msg);
    }

    (void)parse_expect(lexer, TokenType::Semicolon);
    return std::make_unique<StmtLet>(std::move(ids), std::move(tys), std::unique_ptr<Expr>(expr), attrs, std::move(info));
}

std::unique_ptr<StmtExpr>
Parser::parse_stmt_expr(Lexer &lexer) {
    Expr *expr = Parser::parse_expr(lexer);
    parse_expect(lexer, TokenType::Semicolon);
    return std::make_unique<StmtExpr>(std::unique_ptr<Expr>(expr));
}

std::unique_ptr<StmtBlock>
Parser::parse_stmt_block(Lexer &lexer) {
    (void)Parser::parse_expect(lexer, TokenType::Lbrace);

    std::vector<std::unique_ptr<Stmt>> stmts;

    while (lexer.peek(0) && lexer.peek()->type() != TokenType::Rbrace) {
        std::unique_ptr<Stmt> stmt = Parser::parse_stmt(lexer);
        stmts.push_back(std::move(stmt));
    }

    (void)Parser::parse_expect(lexer, TokenType::Rbrace);

    return std::make_unique<StmtBlock>(std::move(stmts));
}

std::vector<std::pair<std::pair<std::shared_ptr<Token>, std::optional<std::shared_ptr<__Type>>>, uint32_t>>
parse_stmt_def_args(Lexer &lexer) {
    std::vector<std::pair<std::pair<std::shared_ptr<Token>, std::optional<std::shared_ptr<__Type>>>, uint32_t>>
        args = {};

    (void)Parser::parse_expect(lexer, TokenType::Lparen);
    while (lexer.peek(0) && lexer.peek()->type() != TokenType::Rparen) {
        uint32_t attr = 0;

        while (lexer.peek(0) && lexer.peek()->type() == TokenType::At)
            attr |= static_cast<uint32_t>(translate_attr(lexer));

        std::shared_ptr<Token> id = Parser::parse_expect(lexer, TokenType::Ident);
        std::shared_ptr<Token> var = std::move(id);
        std::optional<std::shared_ptr<__Type>> ty = {};

        if (is_ty(lexer))
            ty = get_ty(lexer);

        args.push_back(std::make_pair(std::make_pair(std::move(var), std::move(ty)), attr));

        if (lexer.peek(0) && lexer.peek()->type() == TokenType::Comma)
            lexer.discard();
    }
    (void)Parser::parse_expect(lexer, TokenType::Rparen);

    return args;
}

std::unique_ptr<StmtDef>
Parser::parse_stmt_def(Lexer &lexer, uint32_t attrs, std::vector<std::string> info) {
    (void)parse_expect_keyword(lexer, COMMON_EARLKW_FN);

    std::shared_ptr<Token> id = Parser::parse_expect(lexer, TokenType::Ident);
    std::optional<std::shared_ptr<__Type>> ty = {};

    auto args = parse_stmt_def_args(lexer);

    if (lexer.peek(0) && lexer.peek(0)->type() == TokenType::Colon)
        ty = get_ty(lexer);

    std::unique_ptr<StmtBlock> block = Parser::parse_stmt_block(lexer);
    return std::make_unique<StmtDef>(std::move(id),
                                     std::move(args),
                                     std::move(ty),
                                     std::move(block),
                                     attrs,
                                     std::move(info));
}

std::unique_ptr<StmtReturn>
parse_stmt_return(Lexer &lexer) {
    // (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_RETURN);
    auto tok = lexer.next(); // return
    std::optional<std::unique_ptr<Expr>> value = {};
    Expr *expr = Parser::parse_expr(lexer);
    if (expr)
        value = std::unique_ptr<Expr>(expr);
    (void)Parser::parse_expect(lexer, TokenType::Semicolon);
    return std::make_unique<StmtReturn>(std::move(value), tok);
}

std::unique_ptr<StmtWhile>
parse_stmt_while(Lexer &lexer) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_WHILE);
    Expr *expr = Parser::parse_expr(lexer, /*fail_on=*/'{');
    std::unique_ptr<StmtBlock> block = Parser::parse_stmt_block(lexer);
    return std::make_unique<StmtWhile>(std::unique_ptr<Expr>(expr), std::move(block));
}

std::unique_ptr<StmtForeach>
parse_stmt_foreach(Lexer &lexer) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_FOREACH);
    uint32_t attrs = gather_attrs(lexer);
    auto enumerators = parse_comma_sep_idents_as_toks(lexer);
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_IN);
    Expr *expr = Parser::parse_expr(lexer, /*fail_on=*/'{');
    std::unique_ptr<StmtBlock> block = Parser::parse_stmt_block(lexer);
    return std::make_unique<StmtForeach>(std::move(enumerators),
                                         std::unique_ptr<Expr>(expr),
                                         std::move(block),
                                         attrs);
}

std::unique_ptr<StmtFor>
parse_stmt_for(Lexer &lexer) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_FOR);

    std::shared_ptr<Token> enumerator = Parser::parse_expect(lexer, TokenType::Ident);

    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_IN);

    Expr *start_expr = Parser::parse_expr(lexer);
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_TO);
    Expr *end_expr = Parser::parse_expr(lexer, /*fail_on=*/'{');

    std::unique_ptr<StmtBlock> block = Parser::parse_stmt_block(lexer);

    return std::make_unique<StmtFor>(std::move(enumerator),
                                     std::unique_ptr<Expr>(start_expr),
                                     std::unique_ptr<Expr>(end_expr),
                                     std::move(block));
}

std::unique_ptr<Stmt>
parse_stmt_import(Lexer &lexer) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_IMPORT);
    // std::shared_ptr<Token> fp = Parser::parse_expect(lexer, TokenType::Strlit);
    auto fp = Parser::parse_expr(lexer);

    (void)Parser::parse_expect(lexer, TokenType::Semicolon);

    std::optional<std::shared_ptr<Token>> depth = {};
    std::optional<std::shared_ptr<Token>> as = {};
    Token *peek = lexer.peek(0);

    if (peek && peek->type() == TokenType::Keyword &&
        (peek->lexeme() == COMMON_EARLKW_ALMOST ||
         peek->lexeme() == COMMON_EARLKW_FULL)) {
        depth = lexer.next();
    }

    if (lexer.peek(0) && lexer.peek(0)->type() == TokenType::Keyword &&
        lexer.peek(0)->lexeme() == COMMON_EARLKW_AS) {
        lexer.discard(); // as
        as = Parser::parse_expect(lexer, TokenType::Ident);
    }

    return std::make_unique<StmtImport>(std::shared_ptr<Expr>(fp), std::move(depth), std::move(as));
    assert(false && "todo");
}

std::unique_ptr<Stmt>
parse_stmt_mod(Lexer &lexer) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_MODULE);
    std::shared_ptr<Token> id = Parser::parse_expect(lexer, TokenType::Ident);
    return std::make_unique<StmtMod>(std::move(id));
}

static std::vector<std::pair<std::shared_ptr<Token>, std::optional<std::shared_ptr<__Type>>>>
parse_stmt_class_constructor_arguments(Lexer &lexer) {
    std::vector<std::pair<std::shared_ptr<Token>, std::optional<std::shared_ptr<__Type>>>> ids;

    if (lexer.peek(0) && lexer.peek()->type() == TokenType::Lbracket) {
        lexer.discard();

        while (1) {
            // Only needed if no arguments are provided.
            if (lexer.peek(0) && lexer.peek()->type() == TokenType::Rbracket) {
                lexer.discard();
                break;
            }

            auto id = Parser::parse_expect(lexer, TokenType::Ident);
            std::optional<std::shared_ptr<__Type>> ty = {};

            if (is_ty(lexer)) {
                ty = get_ty(lexer);
            }

            ids.push_back(std::make_pair(id, ty));

            if (lexer.peek(0) && lexer.peek()->type() == TokenType::Comma)
                (void)Parser::parse_expect(lexer, TokenType::Comma);
        }
    }

    return ids;

}

std::unique_ptr<StmtClass>
parse_stmt_class(Lexer &lexer, uint32_t attrs, std::vector<std::string> info) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_CLASS);

    std::shared_ptr<Token> class_id = Parser::parse_expect(lexer, TokenType::Ident);

    std::vector<std::unique_ptr<StmtLet>> members;
    std::vector<std::unique_ptr<StmtDef>> methods;

    auto constructor_args = parse_stmt_class_constructor_arguments(lexer);

    (void)Parser::parse_expect(lexer, TokenType::Lbrace);

    uint32_t inclass_attrs = 0;
    while (lexer.peek(0) && lexer.peek()->type() != TokenType::Rbrace) {

        inclass_attrs = 0;
        do {
            if (lexer.peek(0) && lexer.peek()->type() == TokenType::At)
                inclass_attrs |= static_cast<uint32_t>(translate_attr(lexer));
            else
                break;
        } while (inclass_attrs != 0);

        Token *tok = lexer.peek();
        assert(tok);

        switch (tok->type()) {
        case TokenType::Keyword: {
            if (tok->lexeme() == COMMON_EARLKW_LET) {
                members.push_back(Parser::parse_stmt_let(lexer, inclass_attrs));
            }
            else if (tok->lexeme() == COMMON_EARLKW_FN)
                methods.push_back(Parser::parse_stmt_def(lexer, inclass_attrs));
            else {
                Err::err_wtok(tok);
                std::string msg = "invalid keyword specifier `" + tok->lexeme() + "` in class declaration";
                throw ParserException(msg);
            }
        } break;
        default: {
            Err::err_wtok(tok);
            std::string msg = "invalid token type (" + std::to_string((int)tok->type()) + ") in class declaration";
            throw ParserException(msg);
        } break;
        }
    }

    (void)Parser::parse_expect(lexer, TokenType::Rbrace);

    return std::make_unique<StmtClass>(std::move(class_id),
                                       attrs,
                                       std::move(constructor_args),
                                       std::move(members),
                                       std::move(methods),
                                       std::move(info));
}

static std::unique_ptr<StmtMatch::Branch>
parse_branch(Lexer &lexer) {
    std::vector<std::unique_ptr<Expr>> exprs = {};
    std::optional<std::unique_ptr<Expr>> when = {};
    std::unique_ptr<StmtBlock> block = nullptr;

    while (1) {
        exprs.push_back(std::unique_ptr<Expr>(Parser::parse_expr(lexer, /*fail_on=*/'|')));
        if (lexer.peek(0) && lexer.peek()->type() == TokenType::Pipe)
            lexer.discard();
        else
            break;
    }

    if (lexer.peek(0) && lexer.peek()->lexeme() == COMMON_EARLKW_WHEN) {
        (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_WHEN);
        when = std::unique_ptr<Expr>(Parser::parse_expr(lexer));
    }

    (void)Parser::parse_expect(lexer, TokenType::RightArrow);

    block = Parser::parse_stmt_block(lexer);

    return std::make_unique<StmtMatch::Branch>(std::move(exprs), std::move(when), std::move(block));
}

static std::vector<std::unique_ptr<StmtMatch::Branch>>
parse_branches(Lexer &lexer) {
    std::vector<std::unique_ptr<StmtMatch::Branch>> branches;
    while (lexer.peek(0) && lexer.peek()->type() != TokenType::Rbrace)
        branches.push_back(parse_branch(lexer));
    return branches;
}

std::unique_ptr<StmtMatch>
parse_stmt_match(Lexer &lexer) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_MATCH);

    // The expression to match against
    Expr *expr = Parser::parse_expr(lexer, /*fail_on=*/'{');

    (void)Parser::parse_expect(lexer, TokenType::Lbrace);

    // The branches of the match statement
    std::vector<std::unique_ptr<StmtMatch::Branch>> branches = parse_branches(lexer);

    (void)Parser::parse_expect(lexer, TokenType::Rbrace);

    return std::make_unique<StmtMatch>(std::unique_ptr<Expr>(expr), std::move(branches));
}

std::unique_ptr<StmtBreak>
parse_stmt_break(Lexer &lexer) {
    std::shared_ptr<Token> br = Parser::parse_expect_keyword(lexer, COMMON_EARLKW_BREAK);
    (void)Parser::parse_expect(lexer, TokenType::Semicolon);
    return std::make_unique<StmtBreak>(std::move(br));
}

std::unique_ptr<StmtContinue>
parse_stmt_continue(Lexer &lexer) {
    auto tok = lexer.next(); // continue
    Parser::parse_expect(lexer, TokenType::Semicolon);
    return std::make_unique<StmtContinue>(tok);
}

std::unique_ptr<StmtEnum>
parse_stmt_enum(Lexer &lexer, uint32_t attrs, std::vector<std::string> info) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_ENUM);
    std::shared_ptr<Token> id = Parser::parse_expect(lexer, TokenType::Ident);
    std::vector<std::pair<std::shared_ptr<Token>, std::unique_ptr<Expr>>> elems = {};
    Parser::parse_expect(lexer, TokenType::Lbrace);
    while (1) {
        if (lexer.peek(0) && lexer.peek(0)->type() == TokenType::Rbrace) {
            lexer.discard(); // }
            break;
        }
        auto entry_item = Parser::parse_expect(lexer, TokenType::Ident);
        Expr *entry_value = nullptr;
        if (lexer.peek(0) && lexer.peek(0)->type() == TokenType::Equals) {
            lexer.discard(); // =
            entry_value = Parser::parse_expr(lexer);
        }
        elems.push_back(std::make_pair(std::move(entry_item), std::unique_ptr<Expr>(entry_value)));
        if (lexer.peek(0) && lexer.peek(0)->type() == TokenType::Comma)
            lexer.discard(); // ,
        else {
            (void)Parser::parse_expect(lexer, TokenType::Rbrace);
            break;
        }
    }
    return std::make_unique<StmtEnum>(std::move(id), std::move(elems), attrs, std::move(info));
}

static std::unique_ptr<StmtLoop>
parse_stmt_loop(Lexer &lexer) {
    auto tok = Parser::parse_expect_keyword(lexer, COMMON_EARLKW_LOOP);

    auto block = Parser::parse_stmt_block(lexer);

    return std::make_unique<StmtLoop>(std::move(tok), std::move(block));
}

static std::unique_ptr<Stmt>
parse_stmt_bash(Lexer &lexer) {
    (void)Parser::parse_expect(lexer, TokenType::Dollarsign);
    auto expr = Parser::parse_expr(lexer);
    (void)Parser::parse_expect(lexer, TokenType::Semicolon);
    return std::make_unique<StmtBashLiteral>(std::unique_ptr<Expr>(expr));
}

std::unique_ptr<Stmt>
Parser::parse_stmt(Lexer &lexer) {

    uint32_t attrs = 0;
    std::vector<std::string> info = {};

    do {
        Token *tok = lexer.peek();

        switch (tok->type()) {
        case TokenType::Dollarsign: return parse_stmt_bash(lexer);
        case TokenType::Keyword: {
            if (tok->lexeme() == COMMON_EARLKW_FN)
                return parse_stmt_def(lexer, attrs, std::move(info));
            if (tok->lexeme() == COMMON_EARLKW_ENUM)
                return parse_stmt_enum(lexer, attrs, std::move(info));
            if (tok->lexeme() == COMMON_EARLKW_CLASS)
                return parse_stmt_class(lexer, attrs, std::move(info));
            if (tok->lexeme() == COMMON_EARLKW_LET)
                return parse_stmt_let(lexer, attrs, std::move(info));

            if (info.size() > 0) {
                Err::err_wtok(tok);
                const std::string msg = "Info statements are only available for functions, enums, and classes";
                throw ParserException(msg);
            }

            if (tok->lexeme() == COMMON_EARLKW_IF)
                return parse_stmt_if(lexer);
            if (tok->lexeme() == COMMON_EARLKW_RETURN)
                return parse_stmt_return(lexer);
            if (tok->lexeme() == COMMON_EARLKW_BREAK)
                return parse_stmt_break(lexer);
            if (tok->lexeme() == COMMON_EARLKW_WHILE)
                return parse_stmt_while(lexer);
            if (tok->lexeme() == COMMON_EARLKW_FOREACH)
                return parse_stmt_foreach(lexer);
            if (tok->lexeme() == COMMON_EARLKW_FOR)
                return parse_stmt_for(lexer);
            if (tok->lexeme() == COMMON_EARLKW_IMPORT)
                return parse_stmt_import(lexer);
            if (tok->lexeme() == COMMON_EARLKW_MODULE)
                return parse_stmt_mod(lexer);
            if (tok->lexeme() == COMMON_EARLKW_MATCH)
                return parse_stmt_match(lexer);
            if (tok->lexeme() == COMMON_EARLKW_CONTINUE)
                return parse_stmt_continue(lexer);
            if (tok->lexeme() == COMMON_EARLKW_LOOP)
                return parse_stmt_loop(lexer);
            if (tok->lexeme() == COMMON_EARLKW_NONE
                    || tok->lexeme() == COMMON_EARLKW_TRUE
                    || tok->lexeme() == COMMON_EARLKW_FALSE)
                return parse_stmt_expr(lexer);
            Err::err_wtok(tok);
            std::string msg = "invalid keyword `" + tok->lexeme() + "`";
            throw ParserException(msg);
        } break;
        case TokenType::Ident: {
            if (info.size() > 0) {
                Err::err_wtok(tok);
                const std::string msg = "Info statements are only available for variables, functions, enums, and classes";
                throw ParserException(msg);
            }

            // Keeping track of parens to fix an equals sign being in a closure
            // that is defined inside of a function call.
            int paren = 0;
            for (size_t i = 0; lexer.peek(i) && lexer.peek(i) && lexer.peek(i)->type() != TokenType::Semicolon; ++i) {
                if (lexer.peek(i)->type() == TokenType::Lparen)
                    ++paren;
                if (lexer.peek(i)->type() == TokenType::Rparen)
                    --paren;
                if ((lexer.peek(i)->type() == TokenType::Equals
                     || lexer.peek(i)->type() == TokenType::Plus_Equals
                     || lexer.peek(i)->type() == TokenType::Minus_Equals
                     || lexer.peek(i)->type() == TokenType::Asterisk_Equals
                     || lexer.peek(i)->type() == TokenType::Forwardslash_Equals
                     || lexer.peek(i)->type() == TokenType::Percent_Equals
                     || lexer.peek(i)->type() == TokenType::Backtick_Pipe_Equals
                     || lexer.peek(i)->type() == TokenType::Backtick_Ampersand_Equals
                     || lexer.peek(i)->type() == TokenType::Backtick_Caret_Equals)
                    && paren == 0)
                    return parse_stmt_mut(lexer);
            }
            return parse_stmt_expr(lexer);
        } break;
        case TokenType::At: {
            attrs |= static_cast<uint32_t>(translate_attr(lexer));
        } break;
        case TokenType::Info: {
            info.push_back(lexer.next()->lexeme());
        } break;
        case TokenType::Semicolon: {
            Err::err_wtok(tok);
            std::string msg = "expected a statement or expression but got excess semicolon(s)";
            throw ParserException(msg);
        } break;
        default: {
            if (info.size() > 0) {
                Err::err_wtok(tok);
                const std::string msg = "Info statements are only available for variables, functions, enums, and classes";
                throw ParserException(msg);
            }
            return parse_stmt_expr(lexer);
        }
        }
    } while (attrs != 0 || info.size() != 0);

    std::string msg = "A serious internal error has ocured and has gotten to an unreachable case. Something is very wrong";
    throw ParserException(msg);
    return nullptr;
}

std::unique_ptr<Program>
Parser::parse_program(Lexer &lexer, const std::string filepath, std::string from) {
    if ((flags & __VERBOSE) != 0)
        std::cout << "[EARL] parsing file " << filepath << std::endl;

    std::vector<std::unique_ptr<Stmt>> stmts;

    while (lexer.peek(0) && lexer.peek()->type() != TokenType::Eof)
        stmts.push_back(parse_stmt(lexer));

    if (((flags & __CHECK) != 0) || ((flags & __VERBOSE) != 0)) {
        if (from != "")
            std::cout << "[EARL check] (src=" << from << ") ";
        else
            std::cout << "[EARL check] ";
        std::cout << filepath << " .. ok" << std::endl;
    }

    return std::make_unique<Program>(std::move(stmts), filepath);
}
