module Parser where

import Ast
import Token

expect :: [Token] -> TokenType -> (Token, [Token])
expect [] _ = error $ "expect: empty list"
expect (x:xs) t
  | tokenType x == t = (x, xs)
  | otherwise = error $ "expect: expected " ++ strOfTokenType t ++ " but got " ++ strOfTokenType (tokenType x)

expectType :: [Token] -> (Token, [Token])
expectType [] = error "expectType: empty list"
expectType (x:xs)
  | TTyIdType _ <- tokenType x = (x, xs)
  | otherwise = error $ "expectType: expected an IdType but got " ++ show (tokenType x)

parseLetStmt :: [Token] -> (LetStmt, [Token])
parseLetStmt _ = undefined

parseDefStmt :: [Token] -> (DefStmt, [Token])
parseDefStmt [] = error "parseDefStmt: empty list"
parseDefStmt tokens =
  let (_, tokens1) = expect tokens (TTyKeyword KWdDef)
      (id, tokens2) = expect tokens1 TTyIdentifier
      (args, tokens3) = parseArgs tokens2
      (rettype, tokens4) = expectType tokens3
  in undefined
  where
    parseArgs :: [Tokens] -> ([(Token, IdType)], [Tokens])
    parseArgs = undefined

parseStmts' :: [Token] -> ([Token] -> (b, [Token])) -> (b -> Stmt) -> [Stmt]
parseStmts' tokens parseFunc stmtType =
  let (stmt, tokens') = parseFunc tokens
  in [stmtType stmt] ++ parseStmts tokens'

parseStmts :: [Token] -> [Stmt]
parseStmts [] = []
parseStmts tokens@(Token _ (TTyKeyword KWdDef) _ _ _:xs) = parseStmts' tokens parseDefStmt StmtDef
parseStmts tokens@(Token _ (TTyKeyword KWdLet) _ _ _:xs) = parseStmts' tokens parseLetStmt StmtLet
parseStmts _ = error "invalid toplvl statement"

parse :: [Token] -> Program
parse tokens = Program (parseStmts tokens)
