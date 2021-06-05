#ifndef SYNTAX_TREENODE_H
#define SYNTAX_TREENODE_H


#include<list>
#include"lex/lexical.h"
#include"lex/token.h"


using namespace std;

enum class NodeType {
    Program,
    MethodDecl,
    FormalParams,
    Block,
//	Statement,
    LocalVarDecl,
    AssignStmt,
    ReturnStmt,
    IfStmt,
    ReadStmt,
    WriteStmt,
    WhileStmt,
    RepeatStmt,

    PrimaryExpr,
    ActualParams,

    GTR_EXP,
    GEQ_EXP,
    LES_EXP,
    LEQ_EXP,
    EQU_EXP,
    NEQU_EXP,

    /*NUM_GTR_EXP,          // ��ֵ���ڱ��ʽ
    NUM_GEQ_EXP,          // ��ֵ���ڵ���
    NUM_LES_EXP,          // ��ֵС�ڱ��ʽ
    NUM_LEQ_EXP,          // ��ֵС�ڵ���
    NUM_EQU_EXP,          // ��ֵ����

    STR_GTR_EXP,          // �ַ����ıȽ�
    STR_GEQ_EXP,
    STR_LES_EXP,
    STR_LEQ_EXP,
    STR_EQU_EXP,*/

    LOG_OR_EXP,           // �߼�����ʽ
    LOG_AND_EXP,          // �߼�����ʽ

    ADD_EXP,              // �ӷ�
    SUB_EXP,              // ����
    MUL_EXP,              // �˷�
    DIV_EXP,              // ����

    Terminator            // ��ֹ��
};


struct TreeNode {
    NodeType type;
    Token token;
    list<TreeNode *> child;

    const char *getType();

    TreeNode(NodeType _type);

    TreeNode(NodeType _type, const Token &t);

    ~TreeNode();
};


#endif // !SYNTAX_TREENODE_H
