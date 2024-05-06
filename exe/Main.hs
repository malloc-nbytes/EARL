module Main where

import Lexer
import Parser
import Interpreter

filepath :: String
filepath = "/home/zdh/dev/EARL/input.txt"

main :: IO ()
main = do
  src <- readFile filepath
  let tokens = lexFile src filepath 1 1
  let program = parse tokens
  -- print program
  ctx <- interpret program
  print "EARL: Finished interpreting"
