#include "lex/token.h"

#include <utility>

Token::Token() : type(), row(), column() {}

Token::Token(TokenType t, string v, int _line, int _offset) : type(t), value(std::move(v)), row(_line),
                                                              column(_offset) {}