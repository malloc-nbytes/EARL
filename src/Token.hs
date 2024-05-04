module Token where

data Keyword
  = Keyword
  | KWdIf
  | KWdElse
  | KWdThen
  | KWdWhile
  | KWdDef
  | KWdReturn
  | KWdLet
  deriving (Show, Eq)

data IdType
  = IdType
  | IdTyInt
  | IdTyStr
  | IdTyVoid
  deriving (Show, Eq)

data TokenType
  = TokenType
  | TTyEof
  | TTyIdentifier
  | TTyStringLiteral
  | TTyIntegerLiteral
  | TTyKeyword Keyword
  | TTyIdType IdType
  | TTyEquals
  | TTyPlus
  | TTyMinus
  | TTyAsterisk
  | TTyForwardSlash
  | TTyLParen
  | TTyRParen
  | TTyRBracket
  | TTyLBracket
  | TTyRBrace
  | TTyLBrace
  | TTyColon
  | TTySemiColon
  | TTyComma
  | TTyGreaterThan
  | TTyRightArrow
  | TTyDoubleAmpersand
  | TTyDoublePipe
  | TTyDoubleEquals
  | GreaterThanEqual
  | TTyLessThan
  | TTyLessThanEqual
  | TTyNotEqual
  deriving (Show, Eq)

data Token
  = Token
  { tokenLexeme :: String
  , tokenType :: TokenType
  , tokenRow :: Int
  , tokenCol :: Int
  , tokenFp :: String
  } deriving Show

strOfIdType :: IdType -> String
strOfIdType IdTyInt = "IdTyInt"
strOfIdType IdTyStr = "IdTyStr"
strOfIdType IdTyVoid = "IdTyVoid"
strOfIdType _ = error "strOfIdType: unknown IdType"

strOfKeyword :: Keyword -> String
strOfKeyword KWdIf = "KWdIf"
strOfKeyword KWdElse = "KWdElse"
strOfKeyword KWdThen = "KWdThen"
strOfKeyword KWdWhile = "KWdWhile"
strOfKeyword KWdDef = "KWdDef"
strOfKeyword KWdReturn = "KWdReturn"
strOfKeyword KWdLet = "KWdLet"
strOfKeyword _ = error "strOfKeyword: unknown keyword"

strOfTokenType :: TokenType -> String
strOfTokenType TTyEof = "TTyEof"
strOfTokenType TTyIdentifier = "TTyIdentifier"
strOfTokenType TTyStringLiteral = "TTyStringLiteral"
strOfTokenType TTyIntegerLiteral = "TTyIntegerLiteral"
strOfTokenType (TTyKeyword Keyword) = "TTyKeyword"
strOfTokenType (TTyIdType IdType) = "TTyIdType"
strOfTokenType TTyEquals = "TTyEquals"
strOfTokenType TTyPlus = "TTyPlus"
strOfTokenType TTyMinus = "TTyMinus"
strOfTokenType TTyAsterisk = "TTyAsterisk"
strOfTokenType TTyForwardSlash = "TTyForwardSlash"
strOfTokenType TTyLParen = "TTyLParen"
strOfTokenType TTyRParen = "TTyRParen"
strOfTokenType TTyRBracket = "TTyRBracket"
strOfTokenType TTyLBracket = "TTyLBracket"
strOfTokenType TTyRBrace = "TTyRBrace"
strOfTokenType TTyLBrace = "TTyLBrace"
strOfTokenType TTyColon = "TTyColon"
strOfTokenType TTySemiColon = "TTySemiColon"
strOfTokenType TTyComma = "TTyComma"
strOfTokenType TTyGreaterThan = "TTyGreaterThan"
strOfTokenType TTyRightArrow = "TTyRightArrow"
strOfTokenType TTyDoubleAmpersand = "TTyDoubleAmpersand"
strOfTokenType TTyDoublePipe = "TTyDoublePipe"
strOfTokenType TTyDoubleEquals = "TTyDoubleEquals"
strOfTokenType GreaterThanEqual = "GreaterThanEqual"
strOfTokenType TTyLessThanEqual = "TTyLessThanEqual"
strOfTokenType TTyNotEqual = "TTyLessNotEqual"
strOfTokenType TTyLessThan = "TTyLessThan"

strOfTokenType _ = error "strOfTokenType: unknown TokenType"
