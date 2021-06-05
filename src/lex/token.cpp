#include"lex\token.h"

Token::Token() : type(), row(), column() {}

Token::Token(TokenType t, string v, int _line, int _offset) : type(t), value(v), row(_line), column(_offset) {}