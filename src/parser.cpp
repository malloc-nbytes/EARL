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

Expr *parse_expr(Lexer &lexer);

Token *Parser::parse_expect(Lexer &lexer, TokenType expected) {
    Token *tok = lexer.next();
    if (tok->type() != expected) {
        ERR_WARGS(ErrType::Syntax,
                  "parse_expect: expected %d, got %s `%s`",
                  (int)expected, tok->to_str().c_str(), tok->lexeme().c_str());
    }
    return tok;
}

Token *Parser::parse_expect_keyword(Lexer &lexer, std::string expected) {
    Token *tok = lexer.next();

    if (tok->type() != TokenType::Keyword) {
        ERR_WARGS(ErrType::Syntax,
                  "parse_expect_keyword: %s `%s` is not a keyword",
                  tok->to_str().c_str(), tok->lexeme().c_str());
    }
    if (tok->lexeme() != expected) {
        ERR_WARGS(ErrType::Syntax,
                  "parse_expect_keyword: expected keyword `%s`, got %s `%s`",
                  expected.c_str(), tok->to_str().c_str(), tok->lexeme().c_str());
    }

    return tok;
}

Token *Parser::parse_expect_type(Lexer &lexer) {
    Token *tok = lexer.next();
    if (tok->type() != TokenType::Type) {
        ERR_WARGS(ErrType::Syntax,
                  "parse_expect_type: %s `%s` is not a keyword",
                  tok->to_str().c_str(), tok->lexeme().c_str());
    }
    return tok;
}

static std::vector<std::unique_ptr<Expr>> parse_comma_sep_exprs(Lexer &lexer) {
    std::vector<std::unique_ptr<Expr>> exprs;

    (void)Parser::parse_expect(lexer, TokenType::Lparen);

    while (1) {
        // Only needed if no arguments are provided.
        if (lexer.peek()->type() == TokenType::Rparen) {
            break;
        }
        exprs.push_back(std::unique_ptr<Expr>(Parser::parse_expr(lexer)));
        if (lexer.peek()->type() == TokenType::Comma) {
            (void)Parser::parse_expect(lexer, TokenType::Comma);
        }
        else {
            break;
        }
    }

    Parser::parse_expect(lexer, TokenType::Rparen);
    return exprs;
}

static std::optional<std::vector<std::unique_ptr<Expr>>> try_parse_funccall(Lexer &lexer) {
    if (lexer.peek()->type() == TokenType::Lparen) {
        std::vector<std::unique_ptr<Expr>> exprs = parse_comma_sep_exprs(lexer);
        return exprs;
    }

    return {};
}

static Expr *parse_primary_expr(Lexer &lexer) {
    Token *tok = lexer.next();

    switch (tok->type()) {
    case TokenType::Ident: {
        auto exprs = try_parse_funccall(lexer);

        // We are parsing a function call.
        if (exprs.has_value()) {
            return new ExprFuncCall(std::make_unique<Token>(*tok), std::move(exprs.value()));
        }

        return new ExprIdent(std::make_unique<Token>(*tok));
    } break;
    case TokenType::Intlit: {
        return new ExprIntLit(std::make_unique<Token>(*tok));
    } break;
    case TokenType::Strlit: {
        return new ExprStrLit(std::make_unique<Token>(*tok));
    } break;
    default:
        assert(false && "parse_primary_expr: invalid primary expression");
    }
}

static Expr *parse_multiplicative_expr(Lexer &lexer) {
    Expr *lhs = parse_primary_expr(lexer);
    Token *cur = lexer.peek();
    while (cur && (cur->type() == TokenType::Asterisk
                   || cur->type() == TokenType::Forwardslash)) {
        Token *op = lexer.next();
        Expr *rhs = parse_primary_expr(lexer);
        lhs = new ExprBinary(std::unique_ptr<Expr>(lhs),
                             std::make_unique<Token>(*op),
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
        Token *op = lexer.next();
        Expr *rhs = parse_multiplicative_expr(lexer);
        lhs = new ExprBinary(std::unique_ptr<Expr>(lhs),
                             std::make_unique<Token>(*op),
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
        Token *op = lexer.next();
        Expr *rhs = parse_additive_expr(lexer);
        lhs = new ExprBinary(std::unique_ptr<Expr>(lhs),
                             std::make_unique<Token>(*op),
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
        Token *op = lexer.next();
        Expr *rhs = parse_equalitative_expr(lexer);
        lhs = new ExprBinary(std::unique_ptr<Expr>(lhs),
                             std::make_unique<Token>(*op),
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
    bool tok2_if = tok2->type() == TokenType::Keyword && tok2->lexeme() == COMMON_EARLKW_IF;

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

std::unique_ptr<StmtLet> Parser::parse_stmt_let(Lexer &lexer) {
    (void)parse_expect_keyword(lexer, COMMON_EARLKW_LET);
    Token *id = parse_expect(lexer, TokenType::Ident);
    (void)parse_expect(lexer, TokenType::Colon);
    Token *ty = parse_expect_type(lexer);
    (void)parse_expect(lexer, TokenType::Equals);
    Expr *expr = Parser::parse_expr(lexer);
    (void)parse_expect(lexer, TokenType::Semicolon);

    return std::make_unique<StmtLet>(std::make_unique<Token>(*id),
                                     std::make_unique<Token>(*ty),
                                     std::unique_ptr<Expr>(expr));
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

std::vector<std::pair<std::unique_ptr<Token>, std::unique_ptr<Token>>>
parse_stmt_def_args(Lexer &lexer) {
    std::vector<std::pair<std::unique_ptr<Token>, std::unique_ptr<Token>>> args;

    (void)Parser::parse_expect(lexer, TokenType::Lparen);
    while (lexer.peek()->type() != TokenType::Rparen) {
        Token *id = Parser::parse_expect(lexer, TokenType::Ident);
        (void)Parser::parse_expect(lexer, TokenType::Colon);
        Token *type = Parser::parse_expect_type(lexer);
        auto pair = std::make_pair(std::make_unique<Token>(*id), std::make_unique<Token>(*type));
        args.push_back(std::move(pair));

        if (lexer.peek()->type() == TokenType::Comma)
            lexer.discard();
    }
    (void)Parser::parse_expect(lexer, TokenType::Rparen);

    return args;
}

std::unique_ptr<StmtDef> Parser::parse_stmt_def(Lexer &lexer) {
    (void)parse_expect_keyword(lexer, COMMON_EARLKW_DEF);

    Token *id = Parser::parse_expect(lexer, TokenType::Ident);

    std::vector<std::pair<std::unique_ptr<Token>, std::unique_ptr<Token>>> args
        = parse_stmt_def_args(lexer);

    (void)Parser::parse_expect(lexer, TokenType::RightArrow);

    Token *rettype = parse_expect_type(lexer);

    std::unique_ptr<StmtBlock> block = Parser::parse_stmt_block(lexer);
    return std::make_unique<StmtDef>(std::make_unique<Token>(*id),
                                     std::move(args),
                                     std::make_unique<Token>(*rettype),
                                     std::move(block));
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

std::unique_ptr<Stmt> Parser::parse_stmt(Lexer &lexer) {
    Token *tok = lexer.peek();

    switch (tok->type()) {
    case TokenType::Keyword: {
        if (tok->lexeme() == COMMON_EARLKW_LET) {
            return parse_stmt_let(lexer);
        }
        else if (tok->lexeme() == COMMON_EARLKW_DEF) {
            return parse_stmt_def(lexer);
        }
        else if (tok->lexeme() == COMMON_EARLKW_IF) {
            return parse_stmt_if(lexer);
        }
        else if (tok->lexeme() == COMMON_EARLKW_RETURN) {
            return parse_stmt_return(lexer);
        }
        else if (tok->lexeme() == COMMON_EARLKW_WHILE) {
            return parse_stmt_while(lexer);
        }
        else {
            assert(false && "parse_stmt: invalid keyword");
        }
    } break;
    case TokenType::Ident: {
        if (lexer.peek(1)->type() == TokenType::Lparen) {
            return parse_stmt_expr(lexer);
        }
        return parse_stmt_mut(lexer);
    } break;
    default:
        assert(false && "parse_stmt: invalid statement");
    }
}

Program Parser::parse_program(Lexer &lexer) {
    std::vector<std::unique_ptr<Stmt>> stmts;

    while (lexer.peek()->type() != TokenType::Eof) {
        stmts.push_back(parse_stmt(lexer));
    }

    return Program(std::move(stmts));
}
