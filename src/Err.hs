module Err where

import Token

data Err
  = Err
  | ErrorSyntax
  | ErrorUnreachable
  | ErrorFatal
  deriving Show

err :: Err -> String -> Maybe Token -> a
err e msg mtoken =
  case mtoken of
    Just t@(Token tl _ tr tc tfp) ->
      error $ "\n" ++ tfp ++ ":" ++ show tr ++ ":" ++ show tc ++ ":\n" ++ show e ++ ": " ++ msg ++ "\n" ++ show t
    Nothing -> error $ show e ++ ": " ++ msg
