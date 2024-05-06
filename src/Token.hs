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

  -- Interpretter
  | TTyEof
  | TTyIdType IdType
  | TTyKeyword Keyword

  -- Terms
  | TTyIdentifier
  | TTyStringLiteral
  | TTyIntegerLiteral

  -- Binops
  | TTyPlus
  | TTyMinus
  | TTyAsterisk
  | TTyForwardSlash
  | TTyPercent

  -- Assignment
  | TTyEquals
  | TTyPlusEquals
  | TTyMinusEquals
  | TTyAsteriskEquals
  | TTyForwardSlashEquals
  | TTyPercentEquals

  -- Parens
  | TTyLParen
  | TTyRParen
  | TTyRBracket
  | TTyLBracket
  | TTyRBrace
  | TTyLBrace
  | TTyColon

  -- Comparison
  | TTyGreaterThan
  | TTyLessThan
  | TTyGreaterThanEqual
  | TTyLessThanEqual
  | TTyDoubleEquals
  | TTyNotEqual

  -- Logical
  | TTyDoubleAmpersand
  | TTyDoublePipe

  -- Other
  | TTyRightArrow
  | TTySemiColon
  | TTyComma
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
strOfIdType idt = show idt

strOfKeyword :: Keyword -> String
strOfKeyword kw = show kw

strOfTokenType :: TokenType -> String
strOfTokenType t = show t
