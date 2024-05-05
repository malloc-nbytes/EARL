module Parser where

import Ast
import Token
import Err

-- This function 'consumes' the head of
-- a list of tokens, returning (head, tail)
-- only if TokenType matches `tokenType head`.
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

-- Parses a tuple of csv expressions.
-- Expects opening '(' and consumes
-- the closing ')'.
-- Example:
--   (1+1, x, y+z, f())
parseCSVTupleExprs :: [Token] -> ([Expr], [Token])
parseCSVTupleExprs tokens = f tokens []
  where
    f :: [Token] -> [Expr] -> ([Expr], [Token])
    f [] acc = (acc, [])
    f tokens0 acc =
      let (expr, tokens1) = parseExpr tokens0
      in case tokenType (peek tokens1) of
           TTyComma ->
             let (_, tokens2) = expect tokens1 TTyComma
             in f tokens2 (acc ++ [expr])
           TTyRParen ->
             let (_, tokens2) = expect tokens1 TTyRParen
             in (acc ++ [expr], tokens2)
           _ -> (acc, tokens1)

-- Parses a function call.
-- Example:
--   f(1, x+1, g())
parseFuncCall :: [Token] -> (FuncCall, [Token])
parseFuncCall tokens =
  let (pfcId, tokens0) = expect tokens TTyIdentifier
      (exprs, tokens1) = parseCSVTupleExprs tokens0
  in (FuncCall pfcId exprs, tokens1)

-- The outermost first level of expression parsing.
parseExpr :: [Token] -> (Expr, [Token])
parseExpr tokens = let (expr, rest) = parseLogicalExpr tokens in (expr, rest)
  where
    -- The second level of expression parsing.
    -- Handles logical expressions such as
    -- \||, &&, etc.
    parseLogicalExpr :: [Token] -> (Expr, [Token])
    parseLogicalExpr tokens0 =
      let (lhs, tokens1) = parseEqualityExpr tokens0 in aux tokens1 lhs
      where
        aux :: [Token] -> Expr -> (Expr, [Token])
        aux auxTokens@(x:xs) auxLeft
          | tokenType x == TTyDoubleAmpersand || tokenType x == TTyDoublePipe =
            let (right, auxTokens1) = parseEqualityExpr xs
            in aux auxTokens1 (ExprBinary (auxLeft, x, right))
          | otherwise = (auxLeft, auxTokens)

    -- The third level of expression parsing.
    -- Handles equality expressions such as
    -- ==, >=, !=, <, etc.
    parseEqualityExpr :: [Token] -> (Expr, [Token])
    parseEqualityExpr tokens0 =
      let (lhs, tokens1) = parseAdditiveExpr tokens0 in aux tokens1 lhs
      where
        aux :: [Token] -> Expr -> (Expr, [Token])
        aux auxTokens@(x:xs) auxLeft
          | tokenType x == TTyDoubleEquals
            || tokenType x == TTyGreaterThan
            || tokenType x == TTyLessThan
            || tokenType x == TTyGreaterThanEqual
            || tokenType x == TTyLessThanEqual
            || tokenType x == TTyNotEqual =
            let (right, auxTokens1) = parseAdditiveExpr xs
            in aux auxTokens1 (ExprBinary (auxLeft, x, right))
          | otherwise = (auxLeft, auxTokens)

    -- The fourth level of expression parsing.
    -- Handles additive expressions such as
    -- +, -.
    parseAdditiveExpr :: [Token] -> (Expr, [Token])
    parseAdditiveExpr tokens0 =
      let (lhs, tokens1) = parseMultiplicativeExpr tokens0 in aux tokens1 lhs
      where
        aux :: [Token] -> Expr -> (Expr, [Token])
        aux auxTokens@(x:xs) auxLeft
          | tokenType x == TTyPlus || tokenType x == TTyMinus =
            let (right, auxTokens1) = parseMultiplicativeExpr xs
            in aux auxTokens1 (ExprBinary (auxLeft, x, right))
          | otherwise = (auxLeft, auxTokens)

    -- The fifth level of expression parsing.
    -- Handles multiplicative expressions such as
    -- \*, /, % etc.
    parseMultiplicativeExpr :: [Token] -> (Expr, [Token])
    parseMultiplicativeExpr tokens0 =
      let (lhs, tokens1) = parsePrimaryExpr tokens0 in aux tokens1 lhs
      where
        aux :: [Token] -> Expr -> (Expr, [Token])
        aux auxTokens@(x:xs) auxLeft
          | tokenType x == TTyAsterisk
            || tokenType x == TTyForwardSlash
            || tokenType x == TTyPercent =
            let (right, auxTokens1) = parsePrimaryExpr xs
            in aux auxTokens1 (ExprBinary (auxLeft, x, right))
          | otherwise = (auxLeft, auxTokens)

    -- The last level of expression parsing.
    -- Handles terms such as identifiers,
    -- integer literals, string literals etc.
    parsePrimaryExpr :: [Token] -> (Expr, [Token])
    parsePrimaryExpr tokens0@(x:xs) =
      case tokenType x of
        TTyIdentifier ->
          case tokenType (peek xs) of
            TTyLParen ->
              let (funcCall, tokens1) = parseFuncCall tokens0
              in (ExprFuncCall funcCall, tokens1)
            _ -> (ExprTerm (TermIdent x), xs)
        TTyIntegerLiteral -> (ExprTerm (TermIntlit x), xs)
        TTyStringLiteral -> (ExprTerm (TermStrlit x), xs)
        TTyLParen ->
          let (expr, tokens1) = parseExpr xs
              (_, tokens2) = expect tokens1 TTyRParen
          in (expr, tokens2)
        _ -> err ErrorFatal "parsePrimaryExpr: invalid primary expression" $ Just x

-- Parses the `let` statement.
-- Example:
--   let x = 3+1/2;
parseLetStmt :: [Token] -> (LetStmt, [Token])
parseLetStmt tokens =
  let (_, tokens1) = expect tokens (TTyKeyword KWdLet)
      (plsId, tokens2) = expect tokens1 TTyIdentifier
      (_, tokens3) = expect tokens2 TTyColon
      (plsTy, tokens4) = expectType tokens3
      (_, tokens5) = expect tokens4 TTyEquals
      (expr, tokens6) = parseExpr tokens5
      (_, tokens7) = expect tokens6 TTySemiColon
      plsTyActual = tokenToIdType plsTy
  in (LetStmt plsId plsTyActual expr, tokens7)

parseBlockStmt :: [Token] -> (BlockStmt, [Token])
parseBlockStmt tokens = f tokens []
  where
    f :: [Token] -> [Stmt] -> (BlockStmt, [Token])
    f [] acc = (BlockStmt acc, [])
    f (Token _ TTyRBrace _ _ _:xs) acc = (BlockStmt acc, xs)
    f tokens1 acc = let (stmt, rest) = parseStmt tokens1 in f rest (acc ++ [stmt])

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
parseStmt tokens@(Token _ TTyIdentifier _ _ _:xs) =
  case tokenType (peek xs) of
    TTyLParen ->
      let (stmt, tokens1) = parseFuncCall tokens
          (_, tokens2) = expect tokens1 TTySemiColon
      in (StmtExpr (ExprFuncCall stmt), tokens2)
    _ -> error "unimplemented"
parseStmt (x:_) =
  let msg = "invalid statement"
  in err ErrorFatal msg (Just x)

parse :: [Token] -> Program
parse tokens = Program $ f tokens
  where
    f :: [Token] -> [Stmt]
    f [] = []
    f (Token _ TTyEof _ _ _:_) = []
    f tokens1 = let (stmt, rest) = parseStmt tokens1 in [stmt] ++ f rest
