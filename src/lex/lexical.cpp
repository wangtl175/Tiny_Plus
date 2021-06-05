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
    if (slipping()) { // ��ʱcur��һ����Чֵ
        if (cur >= '0' && cur < '9') { // ����һ������
            return getNum();
        } else if (cur == '"') {  // �ַ���
            return getStr();
        } else if (cur == ';' || cur == ',' || cur == '(' || cur == ')'
                   || cur == '+' || cur == '-' || cur == '*' || cur == '/' || cur == '%'
                   || cur == ':' || cur == '=' || cur == '!' || cur == '<' || cur == '>') {  // ����
            return getSym();
        } else if ((cur <= 'Z' && cur >= 'A') || (cur >= 'a' && cur <= 'z')) {  // �ؼ��ʻ���һ��id
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
    int state = 2; // ��4��state��1�ǿ�ʼ
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
    bool comment = false; // �����ע�͵Ļ�����Ϊtrue
    int cbOffset = -1, cbLine = -1;  // ��¼ע�Ϳ�ʼ�ĵط�
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
        } else if (cur == '/') {  // ��Ҫ�ж��Ƿ���ע�Ϳ�ʼ
            char buf[3] = {'\0'}; // һ�ζ�ȡ�����ַ�
            fin.get(buf, 3);
            if (buf[0] == '*' && buf[1] == '*') { // ����ע��
                comment = true;
                cbOffset = offset;
                cbLine = line;
                offset += 3;
            } else {  // ����ע��
                if (buf[1] != '\0') {
                    fin.putback(buf[1]);
                }
                if (buf[0] != '\0') {
                    fin.putback(buf[0]);
                }
                ++offset;
                return true;
            }
        } else if (!comment) { // �ⲻ��ע�ͣ�����һ����Чֵ�ˣ�Ӧ�÷���
            ++offset;
            return true;
        } else {  // ����ע�ͣ���������ʲô������
            if (cur == '*') { // �ж��Ƿ���ע�ͽ���
                char buf[3] = {'\0'};
                fin.get(buf, 3);
                if (buf[0] == '*' && buf[1] == '/') {  // ���ע�ͽ���
                    offset += 3;
                    comment = false;
                } else { // ����Ҫ����fin
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
