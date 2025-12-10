#pragma once

#include <Windows.h>

enum ETokClass {
  Other,
  Constant,
  Identifier,
  BinaryArithOp,
  BinaryRelatOp,
  BinaryLogicOp,
  UnaryLogicOp,
  AssignOp,
  OpenParen,
  CloseParen
};
/// @brief Represents a column definition in the lexical analyzer.
/// This structure holds information about data definitions and types for columns.
struct CD  // column definition
{
  long lDef;  ///< Data definition identifier.
  long lTyp;  ///< Data type identifier.
};
struct tokent {
  int iInComPrio;
  int iInStkPrio;
  ETokClass eClass;
};

namespace lex {
#include "LexTable.h"

const int TOKS_MAX = 128;  // maximum number of tokens
const int VALS_MAX = 256;

const int TOK_UNARY_OPERATOR = 1;
const int TOK_BINARY_OPERATOR = 2;
const int TOK_COMPARISON_OPERATOR = 4;
const int TOK_LOGICAL_OPERATOR = 8;

const int TOK_ABS = 1;
const int TOK_ACOS = 2;
const int TOK_ASIN = 3;
const int TOK_ATAN = 4;
const int TOK_TOSTRING = 5;
const int TOK_COS = 6;
const int TOK_EXP = 7;
const int TOK_TOINTEGER = 8;
const int TOK_LN = 9;
const int TOK_LOG = 10;
const int TOK_SIN = 11;
const int TOK_SQRT = 12;
const int TOK_TAN = 13;
const int TOK_TOREAL = 14;
const int TOK_UNARY_PLUS = 15;
const int TOK_UNARY_MINUS = 16;

const int TOK_INTEGER = 20;
const int TOK_REAL = 21;
const int TOK_LENGTH_OPERAND = 22;
const int TOK_AREA_OPERAND = 23;
const int TOK_STRING = 24;
const int TOK_IDENTIFIER = 25;
const int TOK_EXPONENTIATE = 26;
const int TOK_MULTIPLY = 27;
const int TOK_DIVIDE = 28;
const int TOK_BINARY_PLUS = 29;
const int TOK_BINARY_MINUS = 30;
const int TOK_EQ = 31;
const int TOK_NE = 32;
const int TOK_GT = 33;
const int TOK_GE = 34;
const int TOK_LT = 35;
const int TOK_LE = 36;
const int TOK_AND = 37;
const int TOK_OR = 38;
const int TOK_NOT = 39;
const int TOK_LPAREN = 40;
const int TOK_RPAREN = 41;

static tokent toktbl[] = {
    {.iInComPrio=0, .iInStkPrio=0, .eClass=Other},            // unused
    {.iInComPrio=110, .iInStkPrio=85, .eClass=Other},         // abs
    {.iInComPrio=110, .iInStkPrio=85, .eClass=Other},         // acos
    {.iInComPrio=110, .iInStkPrio=85, .eClass=Other},         // asin
    {.iInComPrio=110, .iInStkPrio=85, .eClass=Other},         // atan
    {.iInComPrio=110, .iInStkPrio=85, .eClass=Other},         // string
    {.iInComPrio=110, .iInStkPrio=85, .eClass=Other},         // cos
    {.iInComPrio=110, .iInStkPrio=85, .eClass=Other},         // exp
    {.iInComPrio=110, .iInStkPrio=85, .eClass=Other},         // int
    {.iInComPrio=110, .iInStkPrio=85, .eClass=Other},         // ln
    {.iInComPrio=110, .iInStkPrio=85, .eClass=Other},         // log
    {.iInComPrio=110, .iInStkPrio=85, .eClass=Other},         // sin
    {.iInComPrio=110, .iInStkPrio=85, .eClass=Other},         // sqrt
    {.iInComPrio=110, .iInStkPrio=85, .eClass=Other},         // tan
    {.iInComPrio=110, .iInStkPrio=85, .eClass=Other},         // real
    {.iInComPrio=110, .iInStkPrio=85, .eClass=Other},         // unary+
    {.iInComPrio=110, .iInStkPrio=85, .eClass=Other},         // unary-
    {.iInComPrio=0, .iInStkPrio=0, .eClass=Other},            // unused
    {.iInComPrio=0, .iInStkPrio=0, .eClass=Other},            // unused
    {.iInComPrio=0, .iInStkPrio=0, .eClass=Other},            // unused
    {.iInComPrio=0, .iInStkPrio=0, .eClass=Constant},         // integer
    {.iInComPrio=0, .iInStkPrio=0, .eClass=Constant},         // real
    {.iInComPrio=0, .iInStkPrio=0, .eClass=Constant},         // length
    {.iInComPrio=0, .iInStkPrio=0, .eClass=Constant},         // area
    {.iInComPrio=0, .iInStkPrio=0, .eClass=Constant},         // string
    {.iInComPrio=0, .iInStkPrio=0, .eClass=Identifier},       // identifier
    {.iInComPrio=80, .iInStkPrio=79, .eClass=BinaryArithOp},  // **
    {.iInComPrio=70, .iInStkPrio=71, .eClass=BinaryArithOp},  // *
    {.iInComPrio=70, .iInStkPrio=71, .eClass=BinaryArithOp},  // /
    {.iInComPrio=60, .iInStkPrio=61, .eClass=BinaryArithOp},  // +
    {.iInComPrio=60, .iInStkPrio=61, .eClass=BinaryArithOp},  // -
    {.iInComPrio=40, .iInStkPrio=41, .eClass=BinaryRelatOp},  // ==
    {.iInComPrio=40, .iInStkPrio=41, .eClass=BinaryRelatOp},  // !=
    {.iInComPrio=40, .iInStkPrio=41, .eClass=BinaryRelatOp},  // >
    {.iInComPrio=40, .iInStkPrio=41, .eClass=BinaryRelatOp},  // >=
    {.iInComPrio=40, .iInStkPrio=41, .eClass=BinaryRelatOp},  // <
    {.iInComPrio=40, .iInStkPrio=41, .eClass=BinaryRelatOp},  // <=
    {.iInComPrio=20, .iInStkPrio=21, .eClass=BinaryLogicOp},  // &
    {.iInComPrio=10, .iInStkPrio=11, .eClass=BinaryLogicOp},  // |
    {.iInComPrio=30, .iInStkPrio=31, .eClass=UnaryLogicOp},   // !
    {.iInComPrio=110, .iInStkPrio=1, .eClass=OpenParen},      // (
    {.iInComPrio=0, .iInStkPrio=0, .eClass=CloseParen}        // )
};

void BreakExpression(int& iLoc, int& iEnd, int* aiTyp, int* aiTokLoc);
void ConvertStringToVal(int iTyp, long lDef, char* szVal, long* lDefReq, void* p);

/// @brief Serialize a value described by a CD descriptor into a textual representation placed in the provided output buffer.
/// @param acVal Pointer to the binary value(s) to convert. For numeric types this points to raw bytes (4 bytes per integer, 8 bytes per double); for strings it points to the character data.
/// @param arCD Pointer to a CD descriptor that describes the value: arCD->lTyp selects the token/type (e.g. TOK_STRING, TOK_INTEGER, TOK_REAL, TOK_LENGTH_OPERAND, TOK_AREA_OPERAND) and arCD->lDef encodes dimension and element length (LOWORD = dimension, HIWORD = element count/length).
/// @param acPic Output character buffer where the textual representation is written. The function writes brackets for arrays/matrices, quotes for strings, and numeric text for integers and reals.
/// @param picBufferSize Size of acPic in bytes. The function uses this to limit writes, but callers should ensure the buffer is large enough for the expected output.
/// @param aiLen Pointer to an int that receives the length of the produced text (number of characters written, not counting a terminating NUL). The function sets *aiLen to the output length; the buffer may or may not be NUL-terminated in all branches.
void ConvertValToString(char*, CD*, char*, size_t picBufferSize, int*);

void ConvertValTyp(int, int, long*, void*);
void EvalTokenStream(char*, int*, long*, int*, void*, size_t bufferSize);
void Init();
void Parse(const char* pszLine);
void ParseStringOperand(const char* pszTok);
LPSTR ScanForChar(char c, LPSTR* ppStr);
LPSTR ScanForString(LPSTR* ppStr, LPSTR pszTerm, LPSTR* ppArgBuf);
int Scan(char* aszTok, size_t bufferSize, const char* pszLine, int& iLP);
LPSTR SkipWhiteSpace(LPSTR pszString);
int TokType(int);
void UnaryOp(int, int*, long*, double*, size_t bufferSize);
void UnaryOp(int, int*, long*, long*, size_t bufferSize);
}  // namespace lex
