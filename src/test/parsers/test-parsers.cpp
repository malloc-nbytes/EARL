#include <vector>

#include "test-parsers.hpp"
#include "test-suite.hpp"

#include "lexer.hpp"
#include "ast.hpp"
#include "parser.hpp"
#include "common.hpp"

static std::vector<std::string> keywords = COMMON_EARLKW_ASCPL;
static std::vector<std::string> types = COMMON_EARLTY_ASCPL;
static std::string comment = "#";

test_errno_t test_parsers_parse_stmt(void) {
    std::string filepath = "test/sample-input/parse-stmt.1.in";

    StmtType tys[] = {
        StmtType::Let,
        StmtType::Let,
        StmtType::Def,
        StmtType::Def,
        StmtType::Def,
        StmtType::Stmt_Expr,
    };

    std::unique_ptr<Lexer> lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);

    int i = 0;
    for (auto &stmt : program.m_stmts) {
        TEST_ASSERT_EQ(stmt->stmt_type(), tys[i], true);
        ++i;
    }

    return TEST_OK;
}

test_errno_t test_parsers_parse_stmt_def(void) {
    std::string filepath = std::string("test/sample-input/parse-stmt-def.1.in");
    std::unique_ptr<Lexer> lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);

    std::string func_names[] = {"func1","func2","func3"};
    std::string rettypes[] = {"void","int","void"};

    int i = 0;
    for (auto &stmt : program.m_stmts) {
        TEST_ASSERT_EQ(stmt->stmt_type(), StmtType::Def, true);
        StmtDef *def = dynamic_cast<StmtDef *>(stmt.get());
        TEST_ASSERT_EQ(def->m_id->lexeme(), func_names[i], true);
        ++i;
    }

    return TEST_OK;
}

test_errno_t test_parsers_parse_stmt_expr(void) {
    std::string filepath = std::string("test/sample-input/parse-stmt-expr.1.in");
    std::unique_ptr<Lexer> lexer = lex_file(filepath.c_str(), keywords, types, comment);
    Program program = Parser::parse_program(lexer);

    std::string func_names[] = {"print","func1","print","func2"};

    int i = 0;
    for (auto &stmt : program.m_stmts) {
        TEST_ASSERT_EQ(stmt->stmt_type(), StmtType::Stmt_Expr, true);

        StmtExpr *stmt_expr = dynamic_cast<StmtExpr *>(stmt.get());
        Expr *expr = stmt_expr->m_expr.get();
        TEST_ASSERT_EQ(expr->get_type(), ExprType::Term, true);

        ExprTerm *expr_term = dynamic_cast<ExprTerm *>(expr);
        TEST_ASSERT_EQ(expr_term->get_term_type(), ExprTermType::Func_Call, true);

        ExprFuncCall *fc = dynamic_cast<ExprFuncCall *>(expr_term);
        TEST_ASSERT_EQ(fc->m_id->lexeme(), func_names[i], true);

        ++i;
    }

    return TEST_OK;
}

test_errno_t test_parsers_parse_stmt_let(void) {
    return TEST_OK;
}

test_errno_t test_parsers_parse_stmt_mut(void) {
    return TEST_OK;
}

test_errno_t test_parsers_parse_expr(void) {
    return TEST_OK;
}

test_errno_t test_parsers_parse_expect_type(void) {
    return TEST_OK;
}

test_errno_t test_parsers_parse_expect_keyword(void) {
    return TEST_OK;
}

test_errno_t test_parsers_parse_expect(void) {
    return TEST_OK;
}

