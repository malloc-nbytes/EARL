module Parser where

import Ast
import Token
import Err

expect :: [Token] -> TokenType -> (Token, [Token])
expect [] _ = error $ "expect: empty list"
expect (x:xs) t
  | tokenType x == t = (x, xs)
  | otherwise =
    let msg = ("expected " ++ strOfTokenType t ++ " but got " ++ strOfTokenType (tokenType x))
    in err ErrorSyntax msg (Just x)

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
parseBlockStmt tokens = f tokens []
  where
    f :: [Token] -> [Stmt] -> (BlockStmt, [Token])
    f [] acc = (BlockStmt acc, [])
    f tokens1@(_:xs) acc = error "todo"

parseDefStmt :: [Token] -> (DefStmt, [Token])
parseDefStmt [] = error "parseDefStmt: empty list"
parseDefStmt tokens =
  let (_, tokens1) = expect tokens (TTyKeyword KWdDef)
      (pdsId, tokens2) = expect tokens1 TTyIdentifier
      (_, tokens3) = expect tokens2 TTyLParen
      (args, tokens4) = parseArgs tokens3 []
      (_, tokens5) = expect tokens4 TTyRParen
      (_, tokens6) = expect tokens5 TTyRightArrow
      (rettype, tokens7) = expectType tokens6
      (_, tokens8) = expect tokens7 TTyLBrace
      (block, tokens9) = parseBlockStmt tokens8
      rettypeActual = tokenToIdType rettype
  in (DefStmt pdsId args rettypeActual block, tokens9)
  where
    parseArgs :: [Token] -> [(Token, IdType)] -> ([(Token, IdType)], [Token])
    parseArgs [] acc = (acc, [])
    parseArgs paTokens@(x:_) acc
      | tokenType x == TTyRParen = (acc, paTokens)
      | tokenType x == TTyIdentifier =
        let (paId, paTokens1) = expect paTokens TTyIdentifier
            (_, paTokens2) = expect paTokens1 TTyColon
            (paTy, paTokens3) = expectType paTokens2
            acc' = acc ++ [(paId, tokenToIdType paTy)]
        in
          if tokenType (peek paTokens3) == TTyComma then
            parseArgs (snd (expect paTokens3 TTyComma)) acc'
          else parseArgs paTokens3 acc'
      | otherwise =
        let msg = "invalid token in definition"
        in err ErrorSyntax msg (Just x)

parseStmt :: [Token] -> (Stmt, [Token])
parseStmt [] = error "parseStmt: no statement"
parseStmt tokens@(Token _ (TTyKeyword KWdLet) _ _ _:_) =
  let (stmt, tokens1) = parseLetStmt tokens
  in (StmtLet stmt, tokens1)
parseStmt tokens@(Token _ (TTyKeyword KWdDef) _ _ _:_) =
  let (stmt, tokens1) = parseDefStmt tokens
  in (StmtDef stmt, tokens1)
parseStmt _ = error $ "invalid statement"

parse :: [Token] -> Program
parse tokens = Program $ f tokens
  where
    f :: [Token] -> [Stmt]
    f [] = []
    f tokens1 = let (stmt, rest) = parseStmt tokens1 in [stmt] ++ f rest

-- parse tokens = Program (parseStmts tokens)
