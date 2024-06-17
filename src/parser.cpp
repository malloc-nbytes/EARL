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
        Err::err_wtok(tok);
        ERR_WARGS(Err::Type::Syntax,
                  "expected %s, got %s `%s`",
                  tokentype_to_str(expected).c_str(), tokentype_to_str(tok->m_type).c_str(), tok->lexeme().c_str());
    }
    return tok;
}

Token *Parser::parse_expect_keyword(Lexer &lexer, std::string expected) {
    Token *tok = lexer.next();

    if (tok->type() != TokenType::Keyword) {
        ERR_WARGS(Err::Type::Syntax,
                  "%s `%s` is not a keyword",
                  tokentype_to_str(tok->type()).c_str(), tok->lexeme().c_str());
    }
    if (tok->lexeme() != expected) {
        ERR_WARGS(Err::Type::Syntax,
                  "expected keyword `%s`, got %s `%s`",
                  expected.c_str(), tokentype_to_str(tok->type()).c_str(), tok->lexeme().c_str());
    }

    return tok;
}

Token *Parser::parse_expect_type(Lexer &lexer) {
    Token *tok = lexer.next();

    if (tok->type() != TokenType::Type && tok->type() != TokenType::TypeList) {
        ERR_WARGS(Err::Type::Syntax,
                  "%s `%s` is not a type",
                  tokentype_to_str(tok->type()).c_str(), tok->lexeme().c_str());
    }
    return tok;
}

// NOTE: It is up to the calling function to consume the '()' or '[]' or '{}' etc.
// This makes this function more reusable.
static std::vector<std::unique_ptr<Expr>> parse_comma_sep_exprs(Lexer &lexer) {
    std::vector<std::unique_ptr<Expr>> exprs;

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

static Expr *parse_primary_expr(Lexer &lexer) {
    Token *tok = lexer.next();

    switch (tok->type()) {
    case TokenType::Ident: {
        Expr *left = nullptr;

        while (1) {
            auto group = try_parse_funccall(lexer);
            if (group.has_value()) {
                left = new ExprFuncCall(std::make_unique<Token>(*tok), std::move(group.value()));
            }
            else {
                left = new ExprIdent(std::make_unique<Token>(*tok));
            }
            if (lexer.peek()->type() == TokenType::Period) {
                lexer.discard();
                Expr *right = Parser::parse_expr(lexer);
                return new ExprGet(std::unique_ptr<Expr>(left), std::unique_ptr<Expr>(right));
            }
            return left;
        }
    } break;
    case TokenType::Intlit: return new ExprIntLit(std::make_unique<Token>(*tok));
    case TokenType::Strlit: return new ExprStrLit(std::make_unique<Token>(*tok));
    case TokenType::Lbracket: {
        std::vector<std::unique_ptr<Expr>> lst = parse_comma_sep_exprs(lexer);
        (void)Parser::parse_expect(lexer, TokenType::Rbracket);
        return new ExprListLit(std::move(lst));
    } break;
    case TokenType::Lparen: {
        Expr *expr = Parser::parse_expr(lexer);
        (void)Parser::parse_expect(lexer, TokenType::Rparen);
        return expr;
    } break;
    default:
        assert(false && "parse_primary_expr: invalid primary expression");
    }
}

static Expr *parse_multiplicative_expr(Lexer &lexer) {
    Expr *lhs = parse_primary_expr(lexer);
    Token *cur = lexer.peek();
    while (cur && (cur->type() == TokenType::Asterisk
                   || cur->type() == TokenType::Forwardslash
                   || cur->type() == TokenType::Percent)) {
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
    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_LET);
    Token *id = parse_expect(lexer, TokenType::Ident);
    (void)parse_expect(lexer, TokenType::Equals);
    Expr *expr = Parser::parse_expr(lexer);
    (void)parse_expect(lexer, TokenType::Semicolon);
    return std::make_unique<StmtLet>(std::make_unique<Token>(*id), std::unique_ptr<Expr>(expr));
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

std::vector<std::unique_ptr<Token>> parse_stmt_def_args(Lexer &lexer) {
    std::vector<std::unique_ptr<Token>> args;

    (void)Parser::parse_expect(lexer, TokenType::Lparen);
    while (lexer.peek()->type() != TokenType::Rparen) {
        Token *id = Parser::parse_expect(lexer, TokenType::Ident);
        auto var = std::make_unique<Token>(*id);
        args.push_back(std::move(var));

        if (lexer.peek()->type() == TokenType::Comma)
            lexer.discard();
    }
    (void)Parser::parse_expect(lexer, TokenType::Rparen);

    return args;
}

std::unique_ptr<StmtDef> Parser::parse_stmt_def(Lexer &lexer, uint32_t attrs) {
    (void)parse_expect_keyword(lexer, COMMON_EARLKW_FN);

    Token *id = Parser::parse_expect(lexer, TokenType::Ident);

    std::vector<std::unique_ptr<Token>> args = parse_stmt_def_args(lexer);

    std::unique_ptr<StmtBlock> block = Parser::parse_stmt_block(lexer);
    return std::make_unique<StmtDef>(std::make_unique<Token>(*id),
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

    Token *enumerator = Parser::parse_expect(lexer, TokenType::Ident);

    (void)Parser::parse_expect_keyword(lexer, COMMON_EARLKW_IN);

    Expr *start_expr = Parser::parse_expr(lexer);
    (void)Parser::parse_expect(lexer, TokenType::Double_Period);
    Expr *end_expr = Parser::parse_expr(lexer);

    std::unique_ptr<StmtBlock> block = Parser::parse_stmt_block(lexer);

    return std::make_unique<StmtFor>(std::make_unique<Token>(*enumerator),
                                     std::unique_ptr<Expr>(start_expr),
                                     std::unique_ptr<Expr>(end_expr),
                                     std::move(block));
}

static FuncAttr translate_attr(Lexer &lexer) {
    (void)Parser::parse_expect(lexer, TokenType::At);

    Token *attr = Parser::parse_expect(lexer, TokenType::Ident);
    if (attr->lexeme() == COMMON_EARLATTR_PUB) {
        return FuncAttr::Pub;
    }
    if (attr->lexeme() == COMMON_EARLATTR_WORLD) {
        return FuncAttr::World;
    }
    else {
        ERR_WARGS(Err::Type::Fatal, "unknown attribute `%s`", attr->lexeme().c_str());
    }
}

std::unique_ptr<Stmt> Parser::parse_stmt(Lexer &lexer) {

    uint32_t attrs = 0;

    do {
        Token *tok = lexer.peek();

        switch (tok->type()) {
        case TokenType::Keyword: {
            if (tok->lexeme() == COMMON_EARLKW_LET) {
                return parse_stmt_let(lexer);
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
            if (tok->lexeme() == COMMON_EARLKW_WHILE) {
                return parse_stmt_while(lexer);
            }
            if (tok->lexeme() == COMMON_EARLKW_FOR) {
                return parse_stmt_for(lexer);
            }
            assert(false && "parse_stmt: invalid keyword");
        } break;
        case TokenType::Ident: {
            if (lexer.peek(1)->type() == TokenType::Lparen) {
                return parse_stmt_expr(lexer);
            }
            return parse_stmt_mut(lexer);
        } break;
        case TokenType::At: {
            attrs |= static_cast<uint32_t>(translate_attr(lexer));
        } break;
        default:
            std::cout << tok->lexeme() << std::endl;
            assert(false && "parse_stmt: invalid statement");
        }
    } while (attrs != 0);

    ERR(Err::Type::Internal,
        "A serious internal error has ocured and has gotten to an unreachable case. Something is very wrong");
    return nullptr;
}

Program Parser::parse_program(Lexer &lexer) {
    std::vector<std::unique_ptr<Stmt>> stmts;

    while (lexer.peek()->type() != TokenType::Eof) {
        stmts.push_back(parse_stmt(lexer));
    }

    return Program(std::move(stmts));
}
