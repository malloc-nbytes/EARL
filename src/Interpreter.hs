module Interpreter where

import qualified Data.Map as Map

import Err
import Token
import Ast

data Variable a
  = Variable
  { variableId :: Token
  , variableTy :: IdType
  , variableEval :: a
  } deriving Show

newtype Ctx a
  = Ctx
  { ctxTbl :: [Map.Map Token (Variable a)]
  } deriving Show

-- Function to print contents of ctxTbl
printCtxTbl :: (Show a, Num a) => Ctx a -> IO ()
printCtxTbl (Ctx tbl) = mapM_ printMap tbl
  where
    printMap :: Show a => Map.Map Token (Variable a) -> IO ()
    printMap mp = do
      putStrLn "---------"
      mapM_ printVar (Map.toList mp)

    printVar :: Show a => (Token, Variable a) -> IO ()
    printVar (token, var) = putStrLn $ "Token: " ++ (tokenLexeme token) ++ ", Variable: " ++ show var

pushScope :: Ctx a -> Ctx a
pushScope ctx = ctx{ctxTbl = [] ++ (ctxTbl ctx)}

popScope :: Ctx a -> Ctx a
popScope (Ctx []) = error "popScope: tried to pop scope when there is none"
popScope ctx@(Ctx (_:xs)) = ctx{ctxTbl = xs}

getVarFromScope :: Token -> Ctx a -> Maybe (Variable a)
getVarFromScope t (Ctx tbl) = f tbl
  where
    f :: [Map.Map Token (Variable a)] -> Maybe (Variable a)
    f [] = Nothing
    f (x:xs) = case Map.lookup t x of
                 Just k -> Just k
                 Nothing -> f xs

isVarInScope :: Token -> Ctx a -> Bool
isVarInScope t ctx = case getVarFromScope t ctx of
                         Just _ -> True
                         Nothing -> False

addVarToScope :: Token -> IdType -> a -> Ctx a -> Ctx a
addVarToScope token varType eval (Ctx tbl) = Ctx $ case tbl of
    [] -> [Map.singleton token var]
    (x:xs) -> Map.insert token var x : xs
  where
    var = Variable token varType eval

evaluateExpr :: Num a => Expr -> Ctx a -> a
evaluateExpr expr ctx =
  case expr of
    ExprTerm (TermIdent tok) ->
      case getVarFromScope tok ctx of
        Just var -> variableEval var
        Nothing -> err ErrorUndeclared ("variable " ++ tokenLexeme tok ++ " is not defined") $ Just tok
    ExprTerm (TermIntlit tok) ->
      let intVal = read (tokenLexeme tok) :: Int -- Extract integer value from token
      in fromIntegral intVal -- Convert to the appropriate type if needed and return
    ExprTerm (TermStrlit tok) -> undefined
    ExprBinary (left, op, right) -> undefined
    ExprFuncCall funcId -> undefined
    _ -> error "evaluateExpr: invalid expression"

evaluateLetStmt :: Num a => LetStmt -> Ctx a -> Ctx a
evaluateLetStmt (LetStmt elsId elsTy elsExpr) ctx =
  if isVarInScope elsId ctx then
    let msg = "variable " ++ tokenLexeme elsId ++ " is already defined"
    in err ErrorRedeclared msg $ Just elsId
  else
    let expr = evaluateExpr elsExpr ctx
    in addVarToScope elsId elsTy expr ctx

evaluateStmt :: Num a => Stmt -> Ctx a -> IO (Ctx a)
evaluateStmt stmt ctx = do
  case stmt of
    StmtDef _ -> error "StmtDef: unimplemented"
    StmtLet sl -> return $ evaluateLetStmt sl ctx
    StmtBlock _ -> error "StmtBlock: unimplemented"
    StmtMut _ -> error "StmtMut: unimplemented"
    StmtExpr _ -> error "StmtExpr: unimplemented"
    _ -> error "evaluateStmt: invalid statement"

evaluateStmts :: Num a => [Stmt] -> Ctx a -> IO (Ctx a)
evaluateStmts [] ctx = return ctx
evaluateStmts (x:xs) ctx = do
  ctx' <- evaluateStmt x ctx
  evaluateStmts xs ctx'

interpret :: (Num a, Show a) => Program -> IO (Ctx a)
interpret program = do
  ctx <- evaluateStmts (extractStmts program) (Ctx [Map.empty])
  printCtxTbl ctx
  return ctx

  where
    extractStmts :: Program -> [Stmt]
    extractStmts (Program stmts) = stmts
