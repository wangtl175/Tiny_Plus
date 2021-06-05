#include "lex/lexical.h"
#include"exceptions.h"

using namespace std;


Lex::Lex(string fileName) throw(IOException): fin(fileName), cur(' '), line(1), offset(0) {
    if (!fin.is_open()) {
        throw IOException(fileName);
    }
}

Lex::~Lex() {
    fin.close();
}

Token Lex::getNextToken(void) throw(Exception) {
    //return Token();
    if (slipping()) { // 此时cur是一个有效值
        if (cur >= '0' && cur < '9') { // 这是一个数字
            return getNum();
        } else if (cur == '"') {  // 字符串
            return getStr();
        } else if (cur == ';' || cur == ',' || cur == '(' || cur == ')'
                   || cur == '+' || cur == '-' || cur == '*' || cur == '/' || cur == '%'
                   || cur == ':' || cur == '=' || cur == '!' || cur == '<' || cur == '>') {  // 符号
            return getSym();
        } else if ((cur <= 'Z' && cur >= 'A') || (cur >= 'a' && cur <= 'z')) {  // 关键词或者一个id
            int o = offset;
            string word = getWord();
            TokenType t = getType(word);
            return Token(t, word, line, o);
            /*if (isKeyword(word)) {
                return Token(Type::KEY, word, line, o);
            } else {
                return Token(Type::ID, word, line, o);
            }*/
        } else {
            throw Exception("illigal input", line, offset);
        }
    } else {
        return Token(TokenType::END, "", -1, -1);
    }
}

Token Lex::getNum() {
    int nline = line, noffset = offset;
    int state = 2; // 共4个state，1是开始
    string num;
    num.push_back(cur);
    while (fin.get(cur)) {
        if (cur >= '0' && cur <= '9') {
            if (state == 3) {
                state = 4;
            }
        } else if (cur == '.') {
            if (state == 2) {
                state = 3;
            } else {
                throw Exception("illegal input", line, offset);
            }
        } else if (cur == ' ' || cur == '\t' || cur == '\n'
                   || cur == '+' || cur == '-' || cur == '*' || cur == '/'
                   || cur == ',' || cur == ';' || cur == ')'
                   || cur == ':' || cur == '=' || cur == '<' || cur == '>') {
            fin.putback(cur);
            break;
        } else {
            throw Exception("illegal input", line, offset);
        }
        num.push_back(cur);
        ++offset;
    }
    if (state != 2 && state != 4) {
        throw Exception("illegal input", line, offset);
    }
    return Token(TokenType::NUM, num, nline, noffset);
}

Token Lex::getStr() {
    string str;
    str.push_back(cur);
    int o = offset;
    while (fin.get(cur)) {
        ++offset;
        str.push_back(cur);
        if (cur == '"') {
            return Token(TokenType::STR, str, line, o);
        }
    }
    throw Exception("\'\"\' expected", line, offset);
}

Token Lex::getSym() {
    string sym;
    char next = fin.peek();
    int o = offset;
    sym.push_back(cur);
    if (sym[0] == ':' || sym[0] == '!') {
        if (next != '=') {
            throw Exception("illegal input", line, offset);
        } else {
            sym.push_back(next);
            fin.get(cur);
            ++offset;
            if (sym[0] == ':') {
                return Token(TokenType::SYM_ASSIGN, sym, line, o);
            } else {
                return Token(TokenType::SYM_NEQU, sym, line, o);
            }
        }
    } else if ((sym[0] == '<' || sym[0] == '>') && (next == '=')) {
        sym.push_back(next);
        fin.get(cur);
        ++offset;
        if (sym[0] == '<') {
            return Token(TokenType::SYM_LEQ, sym, line, o);
        } else {
            return Token(TokenType::SYM_GEQ, sym, line, o);
        }
    }
    switch (sym[0]) {
        case ';':
            return Token(TokenType::SYM_SEMICOLON, sym, line, o);
        case ',':
            return Token(TokenType::SYM_COMMA, sym, line, o);
        case '(':
            return Token(TokenType::SYM_LP, sym, line, o);
        case ')':
            return Token(TokenType::SYM_RP, sym, line, o);
        case '+':
            return Token(TokenType::SYM_ADD, sym, line, o);
        case '-':
            return Token(TokenType::SYM_SUB, sym, line, o);
        case '*':
            return Token(TokenType::SYM_MUL, sym, line, o);
        case '/':
            return Token(TokenType::SYM_DIV, sym, line, o);
            /*case '%':
                return Token(TokenType::SYM_MOD, sym, line, o);*/
        case '<':
            return Token(TokenType::SYM_LT, sym, line, o);
        case '>':
            return Token(TokenType::SYM_GT, sym, line, o);
        default:
            throw Exception("illegal symbol", line, o);
    }
}

TokenType Lex::getType(const string &s) {
    if (s == "WRITE") {
        return TokenType::KEY_WRITE;
    } else if (s == "READ") {
        return TokenType::KEY_READ;
    } else if (s == "IF") {
        return TokenType::KEY_IF;
    } else if (s == "ELSE") {
        return TokenType::KEY_ELSE;
    } else if (s == "RETURN") {
        return TokenType::KEY_RETURN;
    } else if (s == "BEGIN") {
        return TokenType::KEY_BEGIN;
    } else if (s == "END") {
        return TokenType::KEY_END;
    } else if (s == "MAIN") {
        return TokenType::KEY_MAIN;
    } else if (s == "STRING") {
        return TokenType::KEY_STRING;
    } else if (s == "INT") {
        return TokenType::KEY_INT;
    } else if (s == "REAL") {
        return TokenType::KEY_REAL;
    } else if (s == "WHILE") {
        return TokenType::KEY_WHILE;
    } else if (s == "REPEAT") {
        return TokenType::KEY_REPEAT;
    } else if (s == "UNTIL") {
        return TokenType::KEY_UNTIL;
    } else if (s == "OR") {
        return TokenType::KEY_OR;
    } else if (s == "AND") {
        return TokenType::KEY_AND;
    } else {
        return TokenType::ID;
    }
}

bool Lex::slipping() {
    bool comment = false; // 如果是注释的话，则为true
    int cbOffset = -1, cbLine = -1;  // 记录注释开始的地方
    while (fin.get(cur)) {
        if (cur == ' ') {
            ++offset;
        } else if (cur == '\t') {
            offset += 4;
        } else if (cur == '\r') {
            offset = -1;
        } else if (cur == '\n') {
            ++line;
            offset = -1;
        } else if (cur == '/') {  // 需要判断是否是注释开始
            char buf[3] = {'\0'}; // 一次读取两个字符
            fin.get(buf, 3);
            if (buf[0] == '*' && buf[1] == '*') { // 这是注释
                comment = true;
                cbOffset = offset;
                cbLine = line;
                offset += 3;
            } else {  // 不是注释
                if (buf[1] != '\0') {
                    fin.putback(buf[1]);
                }
                if (buf[0] != '\0') {
                    fin.putback(buf[0]);
                }
                ++offset;
                return true;
            }
        } else if (!comment) { // 这不是注释，读到一个有效值了，应该返回
            ++offset;
            return true;
        } else {  // 这是注释，无论遇到什么都跳过
            if (cur == '*') { // 判断是否是注释结束
                char buf[3] = {'\0'};
                fin.get(buf, 3);
                if (buf[0] == '*' && buf[1] == '/') {  // 这段注释结束
                    offset += 3;
                    comment = false;
                } else { // 否则要回退fin
                    if (buf[1] != '\0') {
                        fin.putback(buf[1]);
                    }
                    if (buf[0] != '\0') {
                        fin.putback(buf[0]);
                    }
                    ++offset;
                }
            }
        }
    }
    if (comment) {
        throw Exception("**/ expected", line, offset);
    }
    return false;
}

string Lex::getWord() {
    string word;
    word.push_back(cur);
    while (fin.get(cur)) {
        if ((cur >= 'a' && cur <= 'z') || (cur >= 'A' && cur <= 'Z') || (cur >= '0' && cur <= '9')) {
            ++offset;
            word.push_back(cur);
        } else {
            fin.putback(cur);
            break;
        }
    }
    return word;
}
