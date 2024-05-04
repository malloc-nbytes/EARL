module Ast where

import Token

newtype Program = Program [ToplvlStmt] deriving Show

data ToplvlStmt
  = ToplvlStmt
  | ToplvlDef DefStmt
  | ToplvlLet LetStmt
  deriving Show

data Stmt
  = Stmt
  | StmtDefStmt DefStmt
  | StmtLet LetStmt

data DefStmt
  = DefStmt
  { defId :: Token
  , defArgs :: [(Token, IdType)]
  , defRettype :: IdType
  } deriving Show

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
  | TermIntlit Int
  | TermStrlit String
  deriving Show
