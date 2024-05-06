module Interpreter where

import qualified Data.Map as Map

import Token
import Ast

interpret :: Program -> IO ()
interpret _ = do
  let nv = Map.empty :: Map.Map Token a


  print "EARL: Finished interpreting"
