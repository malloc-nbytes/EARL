#include <iostream>

#include "err.hpp"
#include "token.hpp"

// err.cpp:9:1: error

void Err::err_wtok(Token *tok) {
    std::cerr << tok->m_fp << ':' << tok->m_row << ':' << tok->m_col << '\n';
}

void Err::err_wconflict(Token *tok1, Token *tok2) {
    (void)tok1;
    (void)tok2;
}
