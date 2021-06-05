#ifndef SYNTAX_SYNTAX_H
#define SYNTAX_SYNTAX_H

#include"lex\lexical.h"
#include"syntax\treenode.h"
#include<unordered_set>
#include<vector>

using namespace std;

class Syntax {
public:
    Syntax(Lex *lex) throw(Exception);

    ~Syntax();

    void program();                                     // �ӿ�ʼ���ſ�ʼ�Ƶ����ɹ��򷵻�true

    void display() const;                                     // ��ӡ�﷨��

private:

    void displayIntree(TreeNode *root, int depth) const;

    bool match(unordered_set<TokenType> &expectedSet);  // �жϵ�ǰToken�������Ƿ�������


    TreeNode *MethodDecl();                             // MethodDecl�ķ���
    TreeNode *FormalParams();                           // FormalParams�ķ���
    TreeNode *Block();                                  // Block�ķ�������
    TreeNode *Statement();                              // Statement��������
    TreeNode *LocalVarDecl();                           // LocalVarDecl��������
    TreeNode *AssignStmt();                             // AssignStmt��������
    TreeNode *ReturnStmt();                             // ReturnStmt��������
    TreeNode *IfStmt();                                 // IfStmt��������
    TreeNode *WriteStmt();                              // WriteStmt��������
    TreeNode *ReadStmt();                               // ReadStmt��������
    TreeNode *WhileStmt();                              // WhileStmt��������
    TreeNode *RepeatStmt();                             // RepeatStmt��������

    TreeNode *Semicolon();                              // ��һ��;��


    TreeNode *Expression();                             // Expression��������
    // �߼�����ʽ

    TreeNode *BoolTerm();                               // BoolTerm -> BoolFactor ( AND BoolFactor )*
    // ���߼�����ʽ

    TreeNode *
    BoolFactor();                             // BoolFactor -> ArithmeticExpression ((< | > | <= | >= | ==) ArithmeticExpression)
    // | String... | ( BoolFactor)

    TreeNode *ArithmeticExpression();                   // ArithmeticExpression��������
    TreeNode *MultiplicativeExpr();                     // �˳����ķ�������
    TreeNode *PrimaryExpr();                            // ԭ�ӱ��ʽ�ķ�������
    TreeNode *ActualParams();                           // �����ķ�������

    TreeNode *StringExpression();                       // StringExpression��������


private:
    TreeNode *root;                        // �﷨���ĸ��ڵ�
    vector<Token> tokens;                  // ����Token
    vector<Token>::iterator curToken;      // ��ǰToken
};


#endif // !SYNTAX_SYNTAX_H
