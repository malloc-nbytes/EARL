module Lexer where

import qualified Data.Map as Map
import Data.Char (isDigit, isAlpha, isAlphaNum)
import Token

keywords :: Map.Map String Keyword
keywords = Map.fromList [("if", KWdIf),
                         ("else", KWdElse),
                         ("then", KWdThen),
                         ("while", KWdWhile),
                         ("def", KWdDef),
                         ("return", KWdReturn),
                         ("let", KWdLet)]

idTypes :: Map.Map String IdType
idTypes = Map.fromList [("int", IdTyInt),
                        ("void", IdTyVoid),
                        ("str", IdTyStr)]

isIdType :: [Char] -> Maybe IdType
isIdType key = Map.lookup key idTypes

isKeyword :: [Char] -> Maybe Keyword
isKeyword key = Map.lookup key keywords

consumeUntil :: [Char] -> (Char -> Bool) -> ([Char], [Char])
consumeUntil lst predicate = f lst []
  where
    f :: [Char] -> [Char] -> ([Char], [Char])
    f [] acc = (acc, [])
    f l@(x:xs) acc
      | predicate x = (acc, l)
      | otherwise = f xs (acc ++ [x])

isIgnorable :: Char -> Bool
isIgnorable c = case c of
                  '\t' -> True
                  ' ' -> True
                  _ -> False

lexFile :: [Char] -> String -> Int -> Int -> [Token]
lexFile [] fp r c = [Token "EOF" TTyEof r c fp]
lexFile ('\n':xs) fp r _ = lexFile xs fp (r+1) 1

lexFile (x@'&':x'@'&':xs) fp r c = [Token ([x]++[x']) TTyDoubleAmpersand r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@'|':x'@'|':xs) fp r c = [Token ([x]++[x']) TTyDoublePipe r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@'-':x'@'>':xs) fp r c = [Token ([x]++[x']) TTyRightArrow r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@'>':x'@'=':xs) fp r c = [Token ([x]++[x']) GreaterThanEqual r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@'<':x'@'=':xs) fp r c = [Token ([x]++[x']) TTyLessThanEqual r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@'!':x'@'=':xs) fp r c = [Token ([x]++[x']) TTyNotEqual r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@'<':xs) fp r c = [Token [x] TTyLessThan r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@'=':xs) fp r c = [Token [x] TTyEquals r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@'+':xs) fp r c = [Token [x] TTyPlus r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@'-':xs) fp r c = [Token [x] TTyMinus r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@'*':xs) fp r c = [Token [x] TTyAsterisk r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@'/':xs) fp r c = [Token [x] TTyForwardSlash r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@'(':xs) fp r c = [Token [x] TTyLParen r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@')':xs) fp r c = [Token [x] TTyRParen r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@'[':xs) fp r c = [Token [x] TTyLBracket r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@']':xs) fp r c = [Token [x] TTyRBracket r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@'{':xs) fp r c = [Token [x] TTyLBrace r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@'}':xs) fp r c = [Token [x] TTyRBrace r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@':':xs) fp r c = [Token [x] TTyColon r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@';':xs) fp r c = [Token [x] TTySemiColon r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@',':xs) fp r c = [Token [x] TTyComma r c fp] ++ lexFile xs fp r (c+1)
lexFile (x@'>':xs) fp r c = [Token [x] TTyGreaterThan r c fp] ++ lexFile xs fp r (c+1)

lexFile ('"':xs) fp r c =
  let (str, rest) = consumeUntil xs (== '"')
      len = c + length str + 1
  in [Token str TTyStringLiteral r c fp] ++ lexFile rest fp r len

lexFile lst@(x:xs) fp r c
  | isIgnorable x = lexFile xs fp r (c+1)
  | isDigit x =
    let (num, rest) = consumeUntil lst (not . isDigit)
        len = c + length num  + 1
    in [Token num TTyIntegerLiteral r c fp] ++ lexFile rest fp r len
  | isAlpha x || x == '_' =
    let (ident, rest) = consumeUntil lst (\k -> (not (isAlphaNum k)) && k /= '_')
        len = c + (length ident) + 1
    in case isKeyword ident of
         Nothing -> case isIdType ident of
                      Nothing -> [Token ident TTyIdentifier r c fp] ++ lexFile rest fp r len
                      Just idt -> [Token ident (TTyIdType idt) r c fp] ++ lexFile rest fp r len
         Just kw -> [Token ident (TTyKeyword kw) r c fp] ++ lexFile rest fp r len
  | otherwise = error ("unimplemented char: " ++ [x])

dumpTokens :: [Token] -> IO ()
dumpTokens [] = return ()
dumpTokens (x:xs) = do
  print x
  dumpTokens xs
