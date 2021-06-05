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

    /*NUM_GTR_EXP,          // 数值大于表达式
    NUM_GEQ_EXP,          // 数值大于等于
    NUM_LES_EXP,          // 数值小于表达式
    NUM_LEQ_EXP,          // 数值小于等于
    NUM_EQU_EXP,          // 数值等于

    STR_GTR_EXP,          // 字符串的比较
    STR_GEQ_EXP,
    STR_LES_EXP,
    STR_LEQ_EXP,
    STR_EQU_EXP,*/

    LOG_OR_EXP,           // 逻辑与表达式
    LOG_AND_EXP,          // 逻辑或表达式

    ADD_EXP,              // 加法
    SUB_EXP,              // 减法
    MUL_EXP,              // 乘法
    DIV_EXP,              // 除法

    Terminator            // 终止符
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
