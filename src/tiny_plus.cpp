#include<iostream>
#include"lex\lexical.h"
#include"syntax\syntax.h"
#include"exceptions.h"

using namespace std;

int main() {
    try {
        Lex lex("D:\\vs2019\\compliers\\Tiny_Plus\\src\\a.txt");
        Syntax syntax(&lex);
        syntax.program();
        syntax.display();
        /*while (true) {
            Token t = lex.getNextToken();
            if (t.type == TokenType::END) {
                break;
            }
            if (t.type == TokenType::ID) {
                cout << "ID " << t.value << endl;
            } else if (t.type == TokenType::NUM) {
                cout << "NUM " << t.value << endl;
            } else if (t.type == TokenType::STR) {
                cout << "STR " << t.value << endl;
            } else if (t.type >= TokenType::KEY_WRITE && t.type <= TokenType::KEY_FALSE) {
                cout << "KEY " << t.value << endl;
            } else {
                cout << "SYM " << t.value << endl;
            }
        }*/
    }
    catch (IOException &e) {
        printf("%s\n", e.what());
        //throw;
        return -1;
    } catch (Exception &e) {
        printf("%s\n", e.what());
        return -1;
    }
}