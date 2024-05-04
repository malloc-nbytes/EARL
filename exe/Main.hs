module Main where

import Lexer
import Parser

filepath :: String
filepath = "/home/zdh/dev/EARL/input.txt"

main :: IO ()
main = do
  src <- readFile filepath
  let tokens = lexFile src filepath 1 1
  -- dumpTokens tokens
  let program = parse tokens
  print program
  -- dumpTokens tokens
