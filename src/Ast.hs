module Ast where

import Token

newtype Program = Program [Stmt] deriving Show

data Stmt
  = Stmt
  | StmtDef DefStmt
  | StmtLet LetStmt
  | StmtBlock BlockStmt
  deriving Show

data DefStmt
  = DefStmt
  { defId :: Token
  , defArgs :: [(Token, IdType)]
  , defRettype :: IdType
  , defBlock :: BlockStmt
  } deriving Show

newtype BlockStmt = BlockStmt [Stmt] deriving Show

data LetStmt
  = LetStmt
  { letId :: Token
  , letType :: IdType
  , letExpr :: Expr
  } deriving Show

data Expr
  = Expr
  | ExprTerm Term
  | ExprBinary (Expr, Token, Expr)
  deriving Show

data Term
  = Term
  | TermIdent Token
  | TermIntlit Token
  | TermStrlit Token
  deriving Show
