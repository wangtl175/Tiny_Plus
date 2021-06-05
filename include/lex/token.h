#ifndef LEX_TOKEN_H
#define LEX_TOKEN_H

#include<string>

using namespace std;

enum class TokenType {
    ID,
    NUM,
    STR,
    END,
    // ϸ��keywords
    KEY_WRITE,
    KEY_READ,
    KEY_IF,
    KEY_ELSE,
    KEY_RETURN,
    KEY_BEGIN,
    KEY_END,
    KEY_MAIN,
    KEY_STRING,
    KEY_INT,
    KEY_REAL,
    KEY_WHILE,
    KEY_REPEAT,
    KEY_UNTIL,
    KEY_AND,
    KEY_OR,
    KEY_TRUE,
    KEY_FALSE,

    // ϸ��SYM
    SYM_SEMICOLON,  // ;
    SYM_COMMA,      // ,
    SYM_LP,         // (
    SYM_RP,         // )
    SYM_ADD,        // +
    SYM_SUB,        // -
    SYM_MUL,        // *
    SYM_DIV,        // /
//	SYM_MOD,        // %
    SYM_ASSIGN,     // :=
    SYM_EQU,        // ==
    SYM_NEQU,       // !=
    SYM_LEQ,        // <=
    SYM_GEQ,        // >=
    SYM_LT,         // <
    SYM_GT          // >
};

struct Token {
    TokenType type;
    string value;
    int row; // ��¼λ��
    int column;

    Token();

    Token(TokenType t, string v, int row, int column);
};

#endif // !LEX_TOKEN_H
