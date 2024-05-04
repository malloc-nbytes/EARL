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

tokenToIdType :: Token -> IdType
tokenToIdType token =
  case tokenType token of
    TTyIdType idType -> idType
    _ -> error "tokenToIdType: Token is not of type TTyIdType"

peek :: [Token] -> Token
peek [] = error "peek: empty list"
peek (x:_) = x

parseLetStmt :: [Token] -> (LetStmt, [Token])
parseLetStmt _ = undefined

parseBlockStmt :: [Token] -> (BlockStmt, [Token])
parseBlockStmt _ = undefined

parseDefStmt :: [Token] -> (DefStmt, [Token])
parseDefStmt [] = error "parseDefStmt: empty list"
parseDefStmt tokens =
  let (_, tokens1) = expect tokens (TTyKeyword KWdDef)
      (pdsId, tokens2) = expect tokens1 TTyIdentifier
      (_, tokens3) = expect tokens2 TTyLParen
      (args, tokens4) = parseArgs tokens3 []
      (_, tokens5) = expect tokens4 TTyRightArrow
      (rettype, tokens6) = expectType tokens5
      (_, tokens7) = expect tokens6 TTyRParen
      (block, tokens8) = parseBlockStmt tokens7
      rettypeActual = tokenToIdType rettype
  in (DefStmt pdsId args rettypeActual block, tokens8)
  where
    parseArgs :: [Token] -> [(Token, IdType)] -> ([(Token, IdType)], [Token])
    parseArgs [] acc = (acc, [])
    parseArgs paTokens@(x:xs) acc
      | tokenType x == TTyRParen = (acc, xs)
      | tokenType x == TTyIdentifier =
        let (paId, paTokens1) = expect paTokens TTyIdentifier
            (_, paTokens2) = expect paTokens1 TTyColon
            (paTy, paTokens3) = expectType paTokens2
            acc' = acc ++ [(paId, tokenToIdType paTy)]
        in
          if tokenType (peek paTokens3) == TTyComma then
            parseArgs (snd (expect paTokens3 TTyComma)) acc'
          else parseArgs paTokens3 acc'
      | otherwise = error $ "parseArgs: invalid token: " ++ (strOfTokenType $ tokenType x)

-- def greater(a: int, b: int) -> int {

parseStmts' :: [Token] -> ([Token] -> (b, [Token])) -> (b -> Stmt) -> [Stmt]
parseStmts' tokens parseFunc stmtType =
  let (stmt, tokens') = parseFunc tokens
  in [stmtType stmt] ++ parseStmts tokens'

parseStmts :: [Token] -> [Stmt]
parseStmts [] = []
parseStmts tokens@(Token _ (TTyKeyword KWdDef) _ _ _:_) = parseStmts' tokens parseDefStmt StmtDef
parseStmts tokens@(Token _ (TTyKeyword KWdLet) _ _ _:_) = parseStmts' tokens parseLetStmt StmtLet
parseStmts _ = error "invalid toplvl statement"

parse :: [Token] -> Program
parse tokens = Program (parseStmts tokens)
