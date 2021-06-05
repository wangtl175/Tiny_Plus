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
	TokenType getType(const string &s);    // ��ȡһ���ʵ����ͣ��ؼ��ʻ���id
	bool slipping();                  // �����ո��ע�ͣ��ļ��������򷵻�false
	string getWord();                 // ��cur������һ���ո����Ż���
	Token getNum();                   // ��ȡһ������
	Token getStr();                   // ��ȡ�ַ���
	Token getSym();                   // ��ȡ������
private:
	ifstream fin;              // �����ļ�
	char cur;                  // ��ǰָ�룬�Ѷ�ȡ����ʼ��δ�ո�
	int line;                  // ��¼�����ڼ���
	int offset;                // ��¼�����ͼ����ո�
};

#endif // !LEX_LEXICAL_H
