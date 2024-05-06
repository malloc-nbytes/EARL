module Interpreter where

import qualified Data.Map as Map

import Token
import Ast

data Ctx a
  = Ctx
  { ctxTbl :: [Map.Map Token a]
  }

pushScope :: Ctx a -> Ctx a
pushScope ctx = ctx{ctxTbl = (ctxTbl ctx) ++ []}

popScope :: Ctx a -> Ctx a
popScope _ = undefined

evaluateStmt :: Stmt -> Ctx a -> IO ()
evaluateStmt stmt =
  case stmt of
    StmtDef sd -> undefined
    StmtLet sl -> undefined
    StmtBlock sb -> undefined
    StmtMut sm -> undefined
    StmtExpr se -> undefined

interpret :: Program -> IO ()
interpret program = do
  let namedValues = Map.empty :: Map.Map Token a
  let context = Ctx [namedValues]


  print "EARL: Finished interpreting"
