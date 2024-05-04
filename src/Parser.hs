module Parser where

import Ast
import Token

expect :: [Token] -> TokenType -> (Token, [Token])
expect [] _ = error $ "expect: empty list"
expect (x:xs) t
  | tokenType x == t = (x, xs)
  | otherwise = error $ "expect: expected " ++ strOfTokenType t ++ " but got " ++ strOfTokenType (tokenType x)

parseLetStmt :: [Token] -> (LetStmt, [Token])
parseLetStmt _ = undefined

parseDefStmt :: [Token] -> (DefStmt, [Token])
parseDefStmt _ = undefined

parseToplvlStmts' :: [Token] -> ([Token] -> (b, [Token])) -> (b -> ToplvlStmt) -> [ToplvlStmt]
parseToplvlStmts' tokens parseFunc stmtType =
  let (stmt, tokens') = parseFunc tokens
  in [stmtType stmt] ++ parseToplvlStmts tokens'

parseToplvlStmts :: [Token] -> [ToplvlStmt]
parseToplvlStmts [] = []
parseToplvlStmts tokens@(Token _ (TTyKeyword KWdDef) _ _ _:xs) = parseToplvlStmts' tokens parseDefStmt ToplvlDef
parseToplvlStmts tokens@(Token _ (TTyKeyword KWdLet) _ _ _:xs) = parseToplvlStmts' tokens parseLetStmt ToplvlLet
parseToplvlStmts _ = error "invalid toplvl statement"

parse :: [Token] -> Program
parse tokens = Program (parseToplvlStmts tokens)
