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

    void program();                                     // 从开始符号开始推导，成功则返回true

    void display() const;                                     // 打印语法树

private:

    void displayIntree(TreeNode *root, int depth) const;

    bool match(unordered_set<TokenType> &expectedSet);  // 判断当前Token的类型是否在里面


    TreeNode *MethodDecl();                             // MethodDecl的分析
    TreeNode *FormalParams();                           // FormalParams的分析
    TreeNode *Block();                                  // Block的分析函数
    TreeNode *Statement();                              // Statement分析函数
    TreeNode *LocalVarDecl();                           // LocalVarDecl分析函数
    TreeNode *AssignStmt();                             // AssignStmt分析函数
    TreeNode *ReturnStmt();                             // ReturnStmt分析函数
    TreeNode *IfStmt();                                 // IfStmt分析函数
    TreeNode *WriteStmt();                              // WriteStmt分析函数
    TreeNode *ReadStmt();                               // ReadStmt分析函数
    TreeNode *WhileStmt();                              // WhileStmt分析函数
    TreeNode *RepeatStmt();                             // RepeatStmt分析函数

    TreeNode *Semicolon();                              // 读一个;号


    TreeNode *Expression();                             // Expression分析函数
    // 逻辑或表达式

    TreeNode *BoolTerm();                               // BoolTerm -> BoolFactor ( AND BoolFactor )*
    // 即逻辑与表达式

    TreeNode *
    BoolFactor();                             // BoolFactor -> ArithmeticExpression ((< | > | <= | >= | ==) ArithmeticExpression)
    // | String... | ( BoolFactor)

    TreeNode *ArithmeticExpression();                   // ArithmeticExpression分析函数
    TreeNode *MultiplicativeExpr();                     // 乘除法的分析函数
    TreeNode *PrimaryExpr();                            // 原子表达式的分析函数
    TreeNode *ActualParams();                           // 参数的分析函数

    TreeNode *StringExpression();                       // StringExpression分析函数


private:
    TreeNode *root;                        // 语法树的根节点
    vector<Token> tokens;                  // 所有Token
    vector<Token>::iterator curToken;      // 当前Token
};


#endif // !SYNTAX_SYNTAX_H
