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
#include <optional>

#include "utils.hpp"
#include "err.hpp"
#include "ast.hpp"
#include "common.hpp"
#include "parser.hpp"

std::vector<std::pair<std::unique_ptr<Token>, uint32_t>> parse_stmt_def_args(Lexer &lexer);

std::unique_ptr<Token> Parser::parse_expect(Lexer &lexer, TokenType expected) {
    std::unique_ptr<Token> tok = lexer.next();
    if (tok->type() != expected) {
        Err::err_wtok(tok.get());
        ERR_WARGS(Err::Type::Syntax,
                  "expected %s, got %s `%s`",
                  tokentype_to_str(expected).c_str(), tokentype_to_str(tok->m_type).c_str(), tok->lexeme().c_str());
    }
    return tok;
}

std::unique_ptr<Token> Parser::parse_expect_keyword(Lexer &lexer, std::string expected) {
    std::unique_ptr<Token> tok = lexer.next();

    if (tok->type() != TokenType::Keyword) {
        Err::err_wtok(tok.get());
        ERR_WARGS(Err::Type::Syntax,
                  "%s `%s` is not a keyword",
                  tokentype_to_str(tok->type()).c_str(), tok->lexeme().c_str());
    }
    if (tok->lexeme() != expected) {
        Err::err_wtok(tok.get());
        ERR_WARGS(Err::Type::Syntax,
                  "expected keyword `%s`, got %s `%s`",
                  expected.c_str(), tokentype_to_str(tok->type()).c_str(), tok->lexeme().c_str());
    }

    return tok;
}

static Attr translate_attr(Lexer &lexer) {
    auto errtok = Parser::parse_expect(lexer, TokenType::At);

    std::unique_ptr<Token> attr = Parser::parse_expect(lexer, TokenType::Ident);
    if (attr->lexeme() == COMMON_EARLATTR_PUB) {
        return Attr::Pub;
    }
    if (attr->lexeme() == COMMON_EARLATTR_WORLD) {
        return Attr::World;
    }
    if (attr->lexeme() == COMMON_EARLATTR_REF) {
        return Attr::Ref;
    }
    else {
        Err::err_wtok(errtok.get());
        ERR_WARGS(Err::Type::Fatal, "unknown attribute `%s`", attr->lexeme().c_str());
    }
}

// NOTE: It is up to the calling function to consume the '()' or '[]' or '{}' etc.
// This makes this function more reusable.
static std::vector<std::unique_ptr<Expr>> parse_comma_sep_exprs(Lexer &lexer) {
    std::vector<std::unique_ptr<Expr>> exprs;

    while (1) {
        // Only needed if no arguments are provided.
        if (lexer.peek()->type() == TokenType::Rparen
            || lexer.peek()->type() == TokenType::Rbrace
            || lexer.peek()->type() == TokenType::Rbracket) {
            break;
        }
        exprs.push_back(std::unique_ptr<Expr>(Parser::parse_expr(lexer)));
        if (lexer.peek()->type() == TokenType::Comma)
            (void)Parser::parse_expect(lexer, TokenType::Comma);
        else
            break;
    }

    return exprs;
}

static std::optional<std::vector<std::unique_ptr<Expr>>> try_parse_funccall(Lexer &lexer) {
    if (lexer.peek()->type() == TokenType::Lparen) {
        (void)Parser::parse_expect(lexer, TokenType::Lparen);
        std::vector<std::unique_ptr<Expr>> exprs = parse_comma_sep_exprs(lexer);
        (void)Parser::parse_expect(lexer, TokenType::Rparen);
        return exprs;
    }

    return {};
}

static Expr *parse_identifier_or_funccall(Lexer &lexer) {
    (void)lexer;
    UNIMPLEMENTED("parse_identifier_or_funccall");
    // std::unique_ptr<Token> tok = lexer.next();
    // auto group = try_parse_funccall(lexer);
    // if (group.has_value()) {
    //     return new ExprFuncCall(std::move(tok), std::move(group.value()));
    // }
    // else {
    //     return new ExprIdent(std::move(tok));
    // }
}

static std::vector<std::pair<std::unique_ptr<Token>, uint32_t>> parse_closure_args(Lexer &lexer) {
    std::vector<std::pair<std::unique_ptr<Token>, uint32_t>> args;

    (void)Parser::parse_expect(lexer, TokenType::Pipe);
    while (lexer.peek()->type() != TokenType::Pipe) {
        uint32_t attr = 0;

        while (lexer.peek()->type() == TokenType::At) {
            attr |= static_cast<uint32_t>(translate_attr(lexer));
        }

        std::unique_ptr<Token> id = Parser::parse_expect(lexer, TokenType::Ident);
        std::unique_ptr<Token> var = std::move(id);
        args.push_back(std::make_pair(std::move(var), attr));

        if (lexer.peek()->type() == TokenType::Comma)
            lexer.discard();
    }
    (void)Parser::parse_expect(lexer, TokenType::Pipe);

    return args;
}

static Expr *parse_primary_expr(Lexer &lexer) {
    Token *tok = nullptr;
    Expr *left = nullptr;
    Expr *right = nullptr;

    while (1) {
        switch (lexer.peek()->type()) {
        case TokenType::Ident: {
            left = new ExprIdent(lexer.next());
        } break;
        case TokenType::Lparen: {
            lexer.discard();
            std::vector<std::unique_ptr<Expr>> tuple = parse_comma_sep_exprs(lexer);
            if (left) {
                (void)Parser::parse_expect(lexer, TokenType::Rparen);
                left = new ExprFuncCall(std::unique_ptr<Expr>(left), std::move(tuple));
            }
            else {
                lexer.discard();
                Expr *expr = Parser::parse_expr(lexer);
                (void)Parser::parse_expect(lexer, TokenType::Rparen);
                left = expr;
            }
        } break;
        case TokenType::Period: {
            lexer.discard();
            right = parse_primary_expr(lexer);
            left = new ExprGet(std::unique_ptr<Expr>(left), std::unique_ptr<Expr>(right));
        } break;
        case TokenType::Intlit: {
            left = new ExprIntLit(lexer.next());
        } break;
        case TokenType::Strlit: {
            left = new ExprStrLit(lexer.next());
        } break;
        case TokenType::Charlit: {
            left = new ExprCharLit(lexer.next());
        } break;
        case TokenType::Lbracket: {
            UNIMPLEMENTED("parse_primary_expr:TokenType::Lbracket");
        } break;
        case TokenType::Double_Colon: {
            UNIMPLEMENTED("parse_primary_expr:TokenType::Double_Colon");
        } break;
        case TokenType::Pipe: {
            std::vector<std::pair<std::unique_ptr<Token>, uint32_t>> args = parse_closure_args(lexer);
            auto block = Parser::parse_stmt_block(lexer);
            return new ExprClosure(std::move(args), std::move(block));
        }
        case TokenType::Keyword: {
            if (lexer.peek()->lexeme() == COMMON_EARLKW_WHEN)
                return nullptr;

            std::unique_ptr<Token> kw = lexer.next();
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
                ERR_WARGS(Err::Type::Fatal, "invalid keyword `%s` while parsing primary expression",
                          kw->lexeme().c_str());
            }
        } break;
        default: {
            return left;
        } break;
        }
    }

    assert(false && "unreachable");
    return nullptr; // unreachable
}

static Expr *parse_multiplicative_expr(Lexer &lexer) {
    Expr *lhs = parse_primary_expr(lexer);
    Token *cur = lexer.peek();
    while (cur && (cur->type() == TokenType::Asterisk
                   || cur->type() == TokenType::Forwardslash
                   || cur->type() == TokenType::Percent)) {
        std::unique_ptr<Token> op = lexer.next();
        Expr *rhs = parse_primary_expr(lexer);
        lhs = new ExprBinary(std::unique_ptr<Expr>(lhs),
                             std::move(op),
                             std::unique_ptr<Expr>(rhs));
        cur = lexer.peek();
    }
    return lhs;
}

static Expr *parse_additive_expr(Lexer &lexer) {
    Expr *lhs = parse_multiplicative_expr(lexer);
    Token *cur = lexer.peek();
    while (cur && (cur->type() == TokenType::Plus
                   || cur->type() == TokenType::Minus)) {
        std::unique_ptr<Token> op = lexer.next();
        Expr *rhs = parse_multiplicative_expr(lexer);
        lhs = new ExprBinary(std::unique_ptr<Expr>(lhs),
                             std::move(op),
                             std::unique_ptr<Expr>(rhs));
        cur = lexer.peek();
    }
    return lhs;
}

static Expr *parse_equalitative_expr(Lexer &lexer) {
    Expr *lhs = parse_additive_expr(lexer);
    Token *cur = lexer.peek();
    while (cur && (cur->type() == TokenType::Double_Equals
                   || cur->type() == TokenType::Greaterthan_Equals
                   || cur->type() == TokenType::Greaterthan
                   || cur->type() == TokenType::Lessthan_Equals
                   || cur->type() == TokenType::Lessthan
                   || cur->type() == TokenType::Bang_Equals)) {
        std::unique_ptr<Token> op = lexer.next();
        Expr *rhs = parse_additive_expr(lexer);
        lhs = new ExprBinary(std::unique_ptr<Expr>(lhs),
                             std::move(op),
                             std::unique_ptr<Expr>(rhs));
        cur = lexer.peek();
    }
    return lhs;
}

static Expr *parse_logical_expr(Lexer &lexer) {
    Expr *lhs = parse_equalitative_expr(lexer);
    Token *cur = lexer.peek();
    while (cur && (cur->type() == TokenType::Double_Ampersand
                   || cur->type() == TokenType::Double_Pipe)) {
        std::unique_ptr<Token> op = lexer.next();
        Expr *rhs = parse_equalitative_expr(lexer);
        lhs = new ExprBinary(std::unique_ptr<Expr>(lhs),
                             std::move(op),
                             std::unique_ptr<Expr>(rhs));
        cur = lexer.peek();
    }
    return lhs;
}

Expr *Parser::parse_expr(Lexer &lexer) {
    return parse_logical_expr(lexer);
}

std::unique_ptr<StmtMut> Parser::parse_stmt_mut(Lexer &lexer) {
    Expr *left = Parser::parse_expr(lexer);
    (void)parse_expect(lexer, TokenType::Equals);
    Expr *right = Parser::parse_expr(lexer);
    (void)parse_expect(lexer, TokenType::Semicolon);
    return std::make_unique<StmtMut>(std::unique_ptr<Expr>(left), std::unique_ptr<Expr>(right));
}

std::unique_ptr<StmtIf> Parser::parse_stmt_if(Lexer &lexer) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_IF);

    Expr *expr = Parser::parse_expr(lexer);
    std::unique_ptr<StmtBlock> block = Parser::parse_stmt_block(lexer);

    // Handle the `else if` or `else` blocks if applicable
    std::optional<std::unique_ptr<StmtBlock>> else_ = {};
    Token *tok1 = lexer.peek();
    Token *tok2 = lexer.peek(1);

    bool tok1_else = tok1->type() == TokenType::Keyword && tok1->lexeme() == COMMON_EARLKW_ELSE;
    bool tok2_if = tok2 && (tok2->type() == TokenType::Keyword && tok2->lexeme() == COMMON_EARLKW_IF); // failure

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

std::unique_ptr<StmtLet> Parser::parse_stmt_let(Lexer &lexer, uint32_t attrs) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_LET);
    std::unique_ptr<Token> id = parse_expect(lexer, TokenType::Ident);
    (void)parse_expect(lexer, TokenType::Equals);
    Expr *expr = Parser::parse_expr(lexer);

    if (!expr) {
        Err::err_wtok(lexer.peek());
        ERR_WARGS(Err::Type::Fatal, "invalid token `%s` while parsing primary expression",
                  lexer.peek()->lexeme().c_str());
    }

    (void)parse_expect(lexer, TokenType::Semicolon);
    return std::make_unique<StmtLet>(std::move(id), std::unique_ptr<Expr>(expr), attrs);
}

std::unique_ptr<StmtExpr> Parser::parse_stmt_expr(Lexer &lexer) {
    Expr *expr = Parser::parse_expr(lexer);
    parse_expect(lexer, TokenType::Semicolon);
    return std::make_unique<StmtExpr>(std::unique_ptr<Expr>(expr));
}

std::unique_ptr<StmtBlock> Parser::parse_stmt_block(Lexer &lexer) {
    (void)Parser::parse_expect(lexer, TokenType::Lbrace);

    std::vector<std::unique_ptr<Stmt>> stmts;

    while (lexer.peek()->type() != TokenType::Rbrace) {
        std::unique_ptr<Stmt> stmt = Parser::parse_stmt(lexer);
        stmts.push_back(std::move(stmt));
    }

    (void)Parser::parse_expect(lexer, TokenType::Rbrace);

    return std::make_unique<StmtBlock>(std::move(stmts));
}

std::vector<std::pair<std::unique_ptr<Token>, uint32_t>> parse_stmt_def_args(Lexer &lexer) {
    std::vector<std::pair<std::unique_ptr<Token>, uint32_t>> args;

    (void)Parser::parse_expect(lexer, TokenType::Lparen);
    while (lexer.peek()->type() != TokenType::Rparen) {
        uint32_t attr = 0;

        while (lexer.peek()->type() == TokenType::At) {
            attr |= static_cast<uint32_t>(translate_attr(lexer));
        }

        std::unique_ptr<Token> id = Parser::parse_expect(lexer, TokenType::Ident);
        std::unique_ptr<Token> var = std::move(id);
        args.push_back(std::make_pair(std::move(var), attr));

        if (lexer.peek()->type() == TokenType::Comma)
            lexer.discard();
    }
    (void)Parser::parse_expect(lexer, TokenType::Rparen);

    return args;
}

std::unique_ptr<StmtDef> Parser::parse_stmt_def(Lexer &lexer, uint32_t attrs) {
    (void)parse_expect_keyword(lexer, COMMON_EARLKW_FN);

    std::unique_ptr<Token> id = Parser::parse_expect(lexer, TokenType::Ident);

    auto args = parse_stmt_def_args(lexer);

    std::unique_ptr<StmtBlock> block = Parser::parse_stmt_block(lexer);
    return std::make_unique<StmtDef>(std::move(id),
                                     std::move(args),
                                     std::move(block),
                                     attrs);
}

std::unique_ptr<StmtReturn> parse_stmt_return(Lexer &lexer) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_RETURN);
    Expr *expr = Parser::parse_expr(lexer);
    (void)Parser::parse_expect(lexer, TokenType::Semicolon);
    return std::make_unique<StmtReturn>(std::unique_ptr<Expr>(expr));
}

std::unique_ptr<StmtWhile> parse_stmt_while(Lexer &lexer) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_WHILE);
    Expr *expr = Parser::parse_expr(lexer);
    std::unique_ptr<StmtBlock> block = Parser::parse_stmt_block(lexer);
    return std::make_unique<StmtWhile>(std::unique_ptr<Expr>(expr), std::move(block));
}

std::unique_ptr<StmtFor> parse_stmt_for(Lexer &lexer) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_FOR);

    std::unique_ptr<Token> enumerator = Parser::parse_expect(lexer, TokenType::Ident);

    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_IN);

    Expr *start_expr = Parser::parse_expr(lexer);
    (void)Parser::parse_expect(lexer, TokenType::Double_Period);
    Expr *end_expr = Parser::parse_expr(lexer);

    std::unique_ptr<StmtBlock> block = Parser::parse_stmt_block(lexer);

    return std::make_unique<StmtFor>(std::move(enumerator),
                                     std::unique_ptr<Expr>(start_expr),
                                     std::unique_ptr<Expr>(end_expr),
                                     std::move(block));
}

std::unique_ptr<Stmt> parse_stmt_import(Lexer &lexer) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_IMPORT);
    std::unique_ptr<Token> fp = Parser::parse_expect(lexer, TokenType::Strlit);
    return std::make_unique<StmtImport>(std::move(fp));
}

std::unique_ptr<Stmt> parse_stmt_mod(Lexer &lexer) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_MODULE);
    std::unique_ptr<Token> id = Parser::parse_expect(lexer, TokenType::Ident);
    return std::make_unique<StmtMod>(std::move(id));
}

static std::vector<std::unique_ptr<Token>> parse_stmt_class_constructor_arguments(Lexer &lexer) {
    std::vector<std::unique_ptr<Token>> ids;

    if (lexer.peek()->type() == TokenType::Lbracket) {
        lexer.discard();

        while (1) {
            // Only needed if no arguments are provided.
            if (lexer.peek()->type() == TokenType::Rbracket) {
                lexer.discard();
                break;
            }
            ids.push_back(Parser::parse_expect(lexer, TokenType::Ident));
            if (lexer.peek()->type() == TokenType::Comma)
                (void)Parser::parse_expect(lexer, TokenType::Comma);
        }
    }

    return ids;

}

std::unique_ptr<StmtClass> parse_stmt_class(Lexer &lexer, uint32_t attrs) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_CLASS);

    std::unique_ptr<Token> class_id = Parser::parse_expect(lexer, TokenType::Ident);

    std::vector<std::unique_ptr<StmtLet>> members;
    std::vector<std::unique_ptr<StmtDef>> methods;

    auto constructor_args = parse_stmt_class_constructor_arguments(lexer);

    (void)Parser::parse_expect(lexer, TokenType::Lbrace);

    uint32_t inclass_attrs = 0;
    while (lexer.peek()->type() != TokenType::Rbrace) {

        inclass_attrs = 0;
        do {
            if (lexer.peek()->type() == TokenType::At)
                inclass_attrs |= static_cast<uint32_t>(translate_attr(lexer));
            else
                break;
        } while (inclass_attrs != 0);

        Token *tok = lexer.peek();
        assert(tok);

        switch (tok->type()) {
        case TokenType::Ident: {
            std::unique_ptr<Token> member_id = Parser::parse_expect(lexer, TokenType::Ident);
            (void)Parser::parse_expect(lexer, TokenType::Equals);
            Expr *member_expr = Parser::parse_expr(lexer);
            (void)Parser::parse_expect(lexer, TokenType::Semicolon);
            members.push_back(std::make_unique<StmtLet>(std::move(member_id),
                                                        std::unique_ptr<Expr>(member_expr),
                                                        inclass_attrs));
        } break;
        case TokenType::Keyword: {
            if (tok->lexeme() == COMMON_EARLKW_FN) {
                methods.push_back(Parser::parse_stmt_def(lexer, inclass_attrs));
            }
            else {
                Err::err_wtok(tok);
                ERR_WARGS(Err::Type::Fatal, "invalid keyword specifier (%s) in class declaration",
                          tok->lexeme().c_str());
            }
        } break;
        default: {
            ERR_WARGS(Err::Type::Fatal, "invalid token type (%d) in class declaration", (int)tok->type());
        } break;
        }
    }

    (void)Parser::parse_expect(lexer, TokenType::Rbrace);

    return std::make_unique<StmtClass>(std::move(class_id),
                                       attrs,
                                       std::move(constructor_args),
                                       std::move(members),
                                       std::move(methods));
}

static std::unique_ptr<StmtMatch::Branch> parse_branch(Lexer &lexer) {
    std::vector<std::unique_ptr<Expr>> exprs = {};
    std::optional<std::unique_ptr<Expr>> when = {};
    std::unique_ptr<StmtBlock> block = nullptr;

    while (1) {
        exprs.push_back(std::unique_ptr<Expr>(Parser::parse_expr(lexer)));
        if (lexer.peek()->type() == TokenType::Pipe)
            lexer.discard();
        else
            break;
    }

    if (lexer.peek()->lexeme() == COMMON_EARLKW_WHEN) {
        (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_WHEN);
        when = std::unique_ptr<Expr>(Parser::parse_expr(lexer));
    }

    (void)Parser::parse_expect(lexer, TokenType::RightArrow);

    block = Parser::parse_stmt_block(lexer);

    return std::make_unique<StmtMatch::Branch>(std::move(exprs), std::move(when), std::move(block));
}

static std::vector<std::unique_ptr<StmtMatch::Branch>> parse_branches(Lexer &lexer) {
    std::vector<std::unique_ptr<StmtMatch::Branch>> branches;
    while (lexer.peek()->type() != TokenType::Rbrace) {
        branches.push_back(parse_branch(lexer));
    }
    return branches;
}

std::unique_ptr<StmtMatch> parse_stmt_match(Lexer &lexer) {
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_MATCH);

    // The expression to match against
    Expr *expr = Parser::parse_expr(lexer);

    (void)Parser::parse_expect(lexer, TokenType::Lbrace);

    // The branches of the match statement
    std::vector<std::unique_ptr<StmtMatch::Branch>> branches = parse_branches(lexer);

    (void)Parser::parse_expect(lexer, TokenType::Rbrace);

    return std::make_unique<StmtMatch>(std::unique_ptr<Expr>(expr), std::move(branches));
}

std::unique_ptr<StmtBreak> parse_stmt_break(Lexer &lexer) {
    std::unique_ptr<Token> br = Parser::parse_expect_keyword(lexer, COMMON_EARLKW_BREAK);
    (void)Parser::parse_expect(lexer, TokenType::Semicolon);
    return std::make_unique<StmtBreak>(std::move(br));
}

std::unique_ptr<Stmt> Parser::parse_stmt(Lexer &lexer) {

    uint32_t attrs = 0;

    do {
        Token *tok = lexer.peek();

        switch (tok->type()) {
        case TokenType::Keyword: {
            if (tok->lexeme() == COMMON_EARLKW_LET) {
                return parse_stmt_let(lexer, attrs);
            }
            if (tok->lexeme() == COMMON_EARLKW_FN) {
                return parse_stmt_def(lexer, attrs);
            }
            if (tok->lexeme() == COMMON_EARLKW_IF) {
                return parse_stmt_if(lexer);
            }
            if (tok->lexeme() == COMMON_EARLKW_RETURN) {
                return parse_stmt_return(lexer);
            }
            if (tok->lexeme() == COMMON_EARLKW_BREAK) {
                return parse_stmt_break(lexer);
            }
            if (tok->lexeme() == COMMON_EARLKW_WHILE) {
                return parse_stmt_while(lexer);
            }
            if (tok->lexeme() == COMMON_EARLKW_FOR) {
                return parse_stmt_for(lexer);
            }
            if (tok->lexeme() == COMMON_EARLKW_IMPORT) {
                return parse_stmt_import(lexer);
            }
            if (tok->lexeme() == COMMON_EARLKW_MODULE) {
                return parse_stmt_mod(lexer);
            }
            if (tok->lexeme() == COMMON_EARLKW_CLASS) {
                return parse_stmt_class(lexer, attrs);
            }
            if (tok->lexeme() == COMMON_EARLKW_MATCH) {
                return parse_stmt_match(lexer);
            }
            Err::err_wtok(tok);
            ERR_WARGS(Err::Type::Fatal, "invalid keyword `%s`", tok->lexeme().c_str());
        } break;
        case TokenType::Ident: {
            // Keeping track of parens to fix an equals sign being in a closure
            // that is defined inside of a function call.
            int paren = 0;
            for (size_t i = 0; lexer.peek(i) && lexer.peek(i)->type() != TokenType::Semicolon; ++i) {
                if (lexer.peek(i)->type() == TokenType::Lparen)
                    ++paren;
                if (lexer.peek(i)->type() == TokenType::Rparen)
                    --paren;
                if (lexer.peek(i)->type() == TokenType::Equals && paren == 0) {
                    return parse_stmt_mut(lexer);
                }
            }
            return parse_stmt_expr(lexer);
        } break;
        case TokenType::At: {
            attrs |= static_cast<uint32_t>(translate_attr(lexer));
        } break;
        default: {
            Err::err_wtok(tok);
            ERR_WARGS(Err::Type::Fatal, "invalid statement `%s`", tok->lexeme().c_str());
        }
        }
    } while (attrs != 0);

    ERR(Err::Type::Internal,
        "A serious internal error has ocured and has gotten to an unreachable case. Something is very wrong");
    return nullptr;
}

std::unique_ptr<Program> Parser::parse_program(Lexer &lexer) {
    std::vector<std::unique_ptr<Stmt>> stmts;

    while (lexer.peek()->type() != TokenType::Eof) {
        stmts.push_back(parse_stmt(lexer));
    }

    return std::make_unique<Program>(std::move(stmts));
}
