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

    void run();                                     // 从开始符号开始推导


    void display() const;                                     // 打印语法树
    void writeCode();

private:

    void displayInTree(TreeNode *node, int depth) const;

    bool match(unordered_set<TokenType> &expectedSet);  // 判断当前Token的类型是否在里面

    string newTemp();  // 返回一个新的临时变量 %ti, i从0递增
    string newLabel(); // 返回一个标签, @Li, i从0递增


    TreeNode *Program();                                // Program的分析
    // MethodDecl的分析
    TreeNode *MethodDecl(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int offset);

    // FormalParams的分析
    TreeNode *FormalParams(unordered_map<string, Symbol> &symtable, unsigned int &offset);

    // Block的分析函数
    TreeNode *Block(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int &offset);

    // Statement分析函数
    TreeNode *Statement(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int &offset);

    // LocalVarDecl分析函数, 参数symtable和code都是
    TreeNode *LocalVarDecl(unordered_map<string, Symbol> &symtable, unsigned int &offset, vector<string> &code);

    // AssignStmt分析函数
    TreeNode *AssignStmt(vector<string> &code, unordered_map<string, Symbol> &symtable);

    // ReturnStmt分析函数
    TreeNode *ReturnStmt(vector<string> &code, unordered_map<string, Symbol> &symtable);

    // IfStmt分析函数
    TreeNode *IfStmt(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int &offset);

    // WriteStmt分析函数
    TreeNode *WriteStmt(vector<string> &code, unordered_map<string, Symbol> &symtable);

    // ReadStmt分析函数
    TreeNode *ReadStmt(vector<string> &code, unordered_map<string, Symbol> &symtable);

    // WhileStmt分析函数
    TreeNode *WhileStmt(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int &offset);

    // RepeatStmt分析函数
    TreeNode *RepeatStmt(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int &offset);

    void Semicolon();                              // 读一个;号


    // Expression分析函数, 多一个control，表示是否时控制流语句，如果为true, code应该是局部的，返回的code里，最后两行代码待完善
    TreeNode *Expression(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res, bool control);
    // 逻辑或表达式

    // BoolTerm -> BoolFactor ( AND BoolFactor )*
    TreeNode *BoolTerm(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res, bool control);
    // 即逻辑与表达式

    // BoolFactor -> ArithmeticExpression ((< | > | <= | >= | ==) ArithmeticExpression)
    TreeNode *BoolFactor(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res);
    // | String... | ( BoolFactor)

    // ArithmeticExpression分析函数, code用于增量生成代码, symtable用于检测变量是否已经声明, 还返回一个临时变量res
    // 即A -> .. 中A的综合属性，这个临时变量就不加到符号表里了
    TreeNode *ArithmeticExpression(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res);

    // 乘除法的分析函数
    TreeNode *MultiplicativeExpr(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res);

    // 原子表达式的分析函数
    TreeNode *PrimaryExpr(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res);

    // 参数的分析函数
    TreeNode *ActualParams(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res);

    // 这里的中间代码直接赋值就算了
    TreeNode *StringExpression(string &res);                       // StringExpression分析函数


private:
    ofstream out;                         // 中间代码输出文件
    TreeNode *root;                        // 语法树的根节点
    vector<Token> tokens;                  // 所有Token
    vector<Token>::iterator curToken;      // 当前Token
    unordered_map<string, Symbol_Type> funTable;  // 函数表, key: function name, value: return type
    vector<string> interCode;  // 中间代码
    unordered_map<string, Symbol> globalSym;  // 全局变量
    unsigned int symOffset;

    int tmpCount;  // 临时变量的计数器，用于生成唯一的临时变量
    int labelCount;
};


#endif // !SYNTAX_SYNTAX_H
