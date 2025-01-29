#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ast.h"

static void dump_stmt(const stmt_t *stmt, int pad);

static void padding(int pad) {
    for (int i = 0; i < pad; ++i)
        printf("  ");
}

static void dump_expr_term_identifier(const expr_identifier_t *id, int pad) {
    (void)pad;
    printf("%s", id->identifier->lx);
}

static void dump_expr_term_integer_literal(const expr_integer_literal_t *integer, int pad) {
    (void)pad;
    printf("%s", integer->integer->lx);
}

static void dump_expr_term_string_literal(const expr_string_literal_t *str, int pad) {
    (void)pad;
    printf("\"%s\"", str->string->lx);
}

static void dump_expr_term_character_literal(const expr_character_literal_t *chara, int pad) {
    (void)pad;
    printf("'%s'", chara->character->lx);
}

static void dump_expr_term_function_call(const expr_function_call_t *expr, int pad) {
    dump_expr(expr->left, pad);
    putchar('(');
    for (size_t i = 0; i < expr->args_len; ++i) {
        dump_expr(expr->args[i], pad);
        if (i != expr->args_len-1)
            printf(", ");
    }
    putchar(')');
}

static void dump_expr_term(const expr_term_t *expr, int pad) {
    switch (expr->type) {
    case EXPR_TERM_TYPE_IDENTIFIER: {
        dump_expr_term_identifier(expr->data.identifier, pad);
    } break;
    case EXPR_TERM_TYPE_INTEGER_LITERAL: {
        dump_expr_term_integer_literal(expr->data.integer_literal, pad);
    } break;
    case EXPR_TERM_TYPE_STRING_LITERAL: {
        dump_expr_term_string_literal(expr->data.string_literal, pad);
    } break;
    case EXPR_TERM_TYPE_CHARACTER_LITERAL: {
        dump_expr_term_character_literal(expr->data.character_literal, pad);
    } break;
    case EXPR_TERM_TYPE_FUNCTION_CALL: {
        dump_expr_term_function_call(expr->data.function_call, pad);
    } break;
    default: {
        fprintf(stderr, "error: %s: unknown expression term: %d\n", __FUNCTION__, expr->type);
        exit(1);
    } break;
    }
}

static void dump_expr_binary(expr_binary_t *bin, int pad) {
    printf("(");
    dump_expr(bin->left, pad);
    printf("%s", bin->op->lx);
    dump_expr(bin->right, pad);
    printf(")");
}

void dump_expr(const expr_t *expr, int pad) {
    switch (expr->type) {
    case EXPR_TYPE_BINARY: {
        dump_expr_binary(expr->data.binary, pad);
    } break;
    case EXPR_TYPE_UNARY: {
        assert(0);
    } break;
    case EXPR_TYPE_TERM: {
        dump_expr_term(expr->data.term, pad);
    } break;
    default: {
        fprintf(stderr, "error: %s: unknown expression: %d\n", __FUNCTION__, expr->type);
        exit(1);
    } break;
    }
}

static void dump_stmt_let(const stmt_let_t *let, int pad) {
    printf("LET %s = ", let->identifier->lx);
    dump_expr(let->expr, pad);
}

static void dump_stmt_expr(const stmt_expr_t *stmt, int pad) {
    printf("(stmt_expr) ");
    dump_expr(stmt->expr, pad);
}

static void dump_stmt_block(const stmt_block_t *stmt, int pad) {
    for (size_t i = 0; i < stmt->stmts_len; ++i)
        dump_stmt(stmt->stmts[i], pad+1);
}

static void dump_stmt_fn(const stmt_fn_t *stmt, int pad) {
    printf("fn %s(", stmt->id->lx);
    for (size_t i = 0; i < stmt->params.len; ++i) {
        printf("%s", stmt->params.data[i]->lx);
        if (i != stmt->params.len-1)
            printf(", ");
    }
    printf("){\n");
    dump_stmt_block(stmt->block, pad);
    printf("}");
}

static void dump_stmt_return(stmt_return_t *stmt, int pad) {
    printf("RETURN ");
    dump_expr(stmt->expr, pad);
}

static void dump_stmt(const stmt_t *stmt, int pad) {
    padding(pad);
    switch (stmt->type) {
    case STMT_TYPE_LET: {
        dump_stmt_let(stmt->data.let, pad);
    } break;
    case STMT_TYPE_EXPR: {
        dump_stmt_expr(stmt->data.expr, pad);
    } break;
    case STMT_TYPE_FN: {
        dump_stmt_fn(stmt->data.fn, pad);
    } break;
    case STMT_TYPE_RETURN: {
        dump_stmt_return(stmt->data.ret, pad);
    } break;
    default: {
        fprintf(stderr, "error: %s: unknown stmt: %s\n", __FUNCTION__, stmt_type_to_cstr(stmt->type));
        exit(1);
    } break;
    }
    printf(";\n");
}

void ast_dump(const program_t *prog) {
    for (size_t i = 0; i < prog->stmts_len; ++i) {
        dump_stmt(prog->stmts[i], 0);
        printf("\n");
    }
}

const char *stmt_type_to_cstr(stmt_type_t type) {
    switch (type) {
    case STMT_TYPE_FN: return "STMT_TYPE_FN";
    case STMT_TYPE_LET: return "STMT_TYPE_LET";
    case STMT_TYPE_BLOCK: return "STMT_TYPE_BLOCK";
    case STMT_TYPE_MUT: return "STMT_TYPE_MUT";
    case STMT_TYPE_EXPR: return "STMT_TYPE_EXPR";
    case STMT_TYPE_IF: return "STMT_TYPE_IF";
    case STMT_TYPE_RETURN: return "STMT_TYPE_RETURN";
    case STMT_TYPE_BREAK: return "STMT_TYPE_BREAK";
    case STMT_TYPE_WHILE: return "STMT_TYPE_WHILE";
    case STMT_TYPE_LOOP: return "STMT_TYPE_LOOP";
    case STMT_TYPE_FOR: return "STMT_TYPE_FOR";
    case STMT_TYPE_FOREACH: return "STMT_TYPE_FOREACH";
    case STMT_TYPE_IMPORT: return "STMT_TYPE_IMPORT";
    case STMT_TYPE_MOD: return "STMT_TYPE_MOD";
    case STMT_TYPE_CLASS: return "STMT_TYPE_CLASS";
    case STMT_TYPE_MATCH: return "STMT_TYPE_MATCH";
    case STMT_TYPE_ENUM: return "STMT_TYPE_ENUM";
    case STMT_TYPE_CONTINUE: return "STMT_TYPE_CONTINUE";
    case STMT_TYPE_BASH_LITERAL: return "STMT_TYPE_BASH_LITERAL";
    case STMT_TYPE_INFO: return "STMT_TYPE_INFO";
    case STMT_TYPE_PIPE: return "STMT_TYPE_PIPE";
    case STMT_TYPE_MULTILINE_BASH: return "STMT_TYPE_MULTILINE_BASH";
    case STMT_TYPE_USE: return "STMT_TYPE_USE";
    case STMT_TYPE_EXEC: return "STMT_TYPE_EXEC";
    case STMT_TYPE_WITH: return "STMT_TYPE_WITH";
    default: {
        fprintf(stderr, "unknown stmt: %d\n", type);
        exit(1);
    } break;
    }
}
