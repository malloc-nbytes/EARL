#include <iostream>

#include "err.hpp"
#include "token.hpp"

void Err::err_wtok(Token *tok) {
    std::cerr << tok->m_fp << ':' << tok->m_row << ':' << tok->m_col << '\n';
}

void Err::err_w2tok(Token *tok1, Token *tok2) {
    std::cerr << tok1->m_fp << ':' << tok1->m_row << ':' << tok1->m_col << '\n';
    std::cerr << tok2->m_fp << ':' << tok2->m_row << ':' << tok2->m_col << '\n';
}

void Err::err_wconflict(Token *tok1, Token *tok2) {
    (void)tok1;
    (void)tok2;
}
