#ifndef SYNTAX_SYNTAX_H
#define SYNTAX_SYNTAX_H

#include"lex/lexical.h"
#include"syntax/treenode.h"
#include "syntax/symbol_table.h"
#include<unordered_set>
#include <unordered_map>
#include<vector>
#include <string>
#include <fstream>

using namespace std;


class Syntax {
public:
    Syntax(Lex *lex, const string &fileName) throw(Exception, IOException);

    ~Syntax();

    void run();                                     // �ӿ�ʼ���ſ�ʼ�Ƶ�


    void display() const;                                     // ��ӡ�﷨��
    void writeCode();

private:

    void displayInTree(TreeNode *node, int depth) const;

    bool match(unordered_set<TokenType> &expectedSet);  // �жϵ�ǰToken�������Ƿ�������

    string newTemp();  // ����һ���µ���ʱ���� %ti, i��0����
    string newLabel(); // ����һ����ǩ, @Li, i��0����


    TreeNode *Program();                                // Program�ķ���
    // MethodDecl�ķ���
    TreeNode *MethodDecl(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int offset);

    // FormalParams�ķ���
    TreeNode *FormalParams(unordered_map<string, Symbol> &symtable, unsigned int &offset);

    // Block�ķ�������
    TreeNode *Block(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int &offset);

    // Statement��������
    TreeNode *Statement(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int &offset);

    // LocalVarDecl��������, ����symtable��code����
    TreeNode *LocalVarDecl(unordered_map<string, Symbol> &symtable, unsigned int &offset, vector<string> &code);

    // AssignStmt��������
    TreeNode *AssignStmt(vector<string> &code, unordered_map<string, Symbol> &symtable);

    // ReturnStmt��������
    TreeNode *ReturnStmt(vector<string> &code, unordered_map<string, Symbol> &symtable);

    // IfStmt��������
    TreeNode *IfStmt(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int &offset);

    // WriteStmt��������
    TreeNode *WriteStmt(vector<string> &code, unordered_map<string, Symbol> &symtable);

    // ReadStmt��������
    TreeNode *ReadStmt(vector<string> &code, unordered_map<string, Symbol> &symtable);

    // WhileStmt��������
    TreeNode *WhileStmt(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int &offset);

    // RepeatStmt��������
    TreeNode *RepeatStmt(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int &offset);

    void Semicolon();                              // ��һ��;��


    // Expression��������, ��һ��control����ʾ�Ƿ�ʱ��������䣬���Ϊtrue, codeӦ���Ǿֲ��ģ����ص�code�������д��������
    TreeNode *Expression(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res, bool control);
    // �߼�����ʽ

    // BoolTerm -> BoolFactor ( AND BoolFactor )*
    TreeNode *BoolTerm(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res, bool control);
    // ���߼�����ʽ

    // BoolFactor -> ArithmeticExpression ((< | > | <= | >= | ==) ArithmeticExpression)
    TreeNode *BoolFactor(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res);
    // | String... | ( BoolFactor)

    // ArithmeticExpression��������, code�����������ɴ���, symtable���ڼ������Ƿ��Ѿ�����, ������һ����ʱ����res
    // ��A -> .. ��A���ۺ����ԣ������ʱ�����Ͳ��ӵ����ű�����
    TreeNode *ArithmeticExpression(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res);

    // �˳����ķ�������
    TreeNode *MultiplicativeExpr(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res);

    // ԭ�ӱ��ʽ�ķ�������
    TreeNode *PrimaryExpr(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res);

    // �����ķ�������
    TreeNode *ActualParams(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res);

    // ������м����ֱ�Ӹ�ֵ������
    TreeNode *StringExpression(string &res);                       // StringExpression��������


private:
    ofstream out;                         // �м��������ļ�
    TreeNode *root;                        // �﷨���ĸ��ڵ�
    vector<Token> tokens;                  // ����Token
    vector<Token>::iterator curToken;      // ��ǰToken
    unordered_map<string, Symbol_Type> funTable;  // ������, key: function name, value: return type
    vector<string> interCode;  // �м����
    unordered_map<string, Symbol> globalSym;  // ȫ�ֱ���
    unsigned int symOffset;

    int tmpCount;  // ��ʱ�����ļ���������������Ψһ����ʱ����
    int labelCount;
};


#endif // !SYNTAX_SYNTAX_H
