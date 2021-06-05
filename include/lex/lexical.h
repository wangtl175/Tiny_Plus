#ifndef LEX_LEXICAL_H
#define LEX_LEXICAL_H

#include<fstream>
#include<string>
#include"lex\token.h"
#include"exceptions.h"

using namespace std;


class Lex {
public:
	Lex(string fileName) throw(IOException);
	~Lex();
	Token getNextToken(void) throw(Exception);
private:
	TokenType getType(const string &s);    // 获取一个词的类型，关键词或者id
	bool slipping();                  // 跳过空格和注释，文件结束了则返回false
	string getWord();                 // 从cur读到第一个空格或符号或换行
	Token getNum();                   // 获取一个数字
	Token getStr();                   // 获取字符串
	Token getSym();                   // 获取操作符
private:
	ifstream fin;              // 代码文件
	char cur;                  // 当前指针，已读取，初始化未空格
	int line;                  // 记录读到第几行
	int offset;                // 记录读到低级个空格
};

#endif // !LEX_LEXICAL_H
