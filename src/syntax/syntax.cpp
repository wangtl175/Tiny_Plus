#include "syntax/syntax.h"
#include"exceptions.h"
#include<iostream>
#include<iomanip>
#include<stack>
#include<memory>

using namespace std;

Syntax::Syntax(Lex *lex, const string &fileName) throw(Exception, IOException): root(nullptr), out(fileName),
                                                                                tmpCount(0), labelCount(0),
                                                                                symOffset(0) {
    if (!out.is_open()) {
        throw IOException(fileName);
    }
    while (true) {
        Token t = lex->getNextToken();
        tokens.emplace_back(t);
        if (t.type == TokenType::END) {
            break;
        }
    }
    curToken = tokens.begin();
}

Syntax::~Syntax() {
    out.close();
    delete root;
}


// 打印一颗子树
void Syntax::displayInTree(TreeNode *node, int depth) const {
    if (node->type == NodeType::PrimaryExpr || node->type == NodeType::Terminator) {
        cout << setw(15) << left << node->token.value;
    } else {
        cout << setw(15) << left << node->getType();
    }
    if (node->child.empty()) {
        cout << endl;
    } else {
        displayInTree(node->child.front(), depth + 1);  // 打印第一个子节点
        list<TreeNode *>::const_iterator cIter = node->child.cbegin();
        for (++cIter; cIter != node->child.cend(); ++cIter) {  // 打印其余的子节点
            for (int i = 0; i < depth + 1; ++i) {
                cout << "               ";
            }
            displayInTree(*cIter, depth + 1);
        }
    }
}

bool Syntax::match(unordered_set<TokenType> &expectedSet) {
    if (expectedSet.find(curToken->type) != expectedSet.end()) {
        return true;
    }
    return false;
}

// 开始
void Syntax::run() {
    root = Program();
}

void Syntax::display() const {
    displayInTree(root, 0);
}

// 这里传进去的是全局变量
TreeNode *Syntax::MethodDecl(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int offset) {
    unique_ptr<TreeNode> node(new TreeNode(NodeType::MethodDecl));
    TokenType retType = curToken->type;
    node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));  // 返回类型
    ++curToken;
    if (curToken->type == TokenType::ID || curToken->type == TokenType::KEY_MAIN) {
        node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
    } else {
        throw Exception("ID or MAIN expected", curToken->row, curToken->column);
    }
    if (funTable.find(curToken->value) != funTable.end()) {  // 重定义
        throw Exception("method redeclare", curToken->row, curToken->column);
    }
    string &funName = curToken->value;
    switch (retType) {
        case TokenType::KEY_INT:
            funTable[funName] = Symbol_Type::INT;
            break;
        case TokenType::KEY_REAL:
            funTable[funName] = Symbol_Type::REAL;
            break;
        case TokenType::KEY_BOOLEN:
            funTable[funName] = Symbol_Type::BOOLEN;
            break;
        default:
            funTable[funName] = Symbol_Type::STRING;
    }
    ++curToken;
    if (curToken->type != TokenType::SYM_LP) {
        throw Exception("'(' expected", curToken->row, curToken->column);
    }
    ++curToken;
    unordered_map<string, Symbol> methodSym(symtable);
    if (curToken->type != TokenType::SYM_RP) {
        node->child.emplace_back(FormalParams(methodSym, offset));
        if (curToken->type != TokenType::SYM_RP) {
            throw Exception("')' expected", curToken->row, curToken->column);
        }
    }
    ++curToken;
    if (curToken->type != TokenType::KEY_BEGIN) {
        throw Exception("BEGIN expected", curToken->row, curToken->column);
    }
    code.emplace_back(funName + ":");
    node->child.emplace_back(Block(code, methodSym, offset));
    return node.release();
}

// 这里要添加一些变量
TreeNode *Syntax::FormalParams(unordered_map<string, Symbol> &symtable, unsigned int &offset) {
    unordered_set<TokenType> typeSet = {TokenType::KEY_INT, TokenType::KEY_REAL, TokenType::KEY_STRING,
                                        TokenType::KEY_BOOLEN};
    unique_ptr<TreeNode> node(new TreeNode(NodeType::FormalParams));
    while (match(typeSet)) {
        Symbol_Type symType;
        unsigned int size;
        switch (curToken->type) {
            case TokenType::KEY_INT:
                symType = Symbol_Type::INT;
                size = INT_SIZE;
                break;
            case TokenType::KEY_REAL:
                symType = Symbol_Type::REAL;
                size = REAL_SIZE;
                break;
            case TokenType::KEY_BOOLEN:
                symType = Symbol_Type::BOOLEN;
                size = BOOLEN_SIZE;
                break;
            default:
                symType = Symbol_Type::STRING;
                size = STRING_SIZE;
                break;
        }
        node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
        ++curToken;
        if (curToken->type != TokenType::ID) {
            throw Exception("ID expected", curToken->row, curToken->column);
        }
        if (symtable.find(curToken->value) != symtable.end()) {
            throw Exception("parameter redefined", curToken->row, curToken->column);
        }
        symtable[curToken->value] = Symbol{offset, size, symType};
        offset += size;
        node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
        ++curToken;
        if (curToken->type == TokenType::SYM_COMMA) {
            ++curToken;
        }
    }
    return node.release();
}

TreeNode *Syntax::Block(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int &offset) {
    unordered_map<string, Symbol> tmpTable(symtable);  // 局部的变量
    // offset不用局部的，是因为局部变量也要分配内存
    unique_ptr<TreeNode> node(new TreeNode(NodeType::Block));
    ++curToken;
    unordered_set<TokenType> stmtSet = {TokenType::KEY_BEGIN, TokenType::KEY_INT, TokenType::KEY_STRING,
                                        TokenType::KEY_REAL, TokenType::ID, TokenType::KEY_RETURN, TokenType::KEY_IF,
                                        TokenType::KEY_WRITE,
                                        TokenType::KEY_READ, TokenType::KEY_WHILE, TokenType::KEY_REPEAT};
    while (match(stmtSet)) {
        node->child.emplace_back(Statement(code, tmpTable, offset));
    }
    if (curToken->type != TokenType::KEY_END) {
        throw Exception("END expected", curToken->row, curToken->column);
    }
    ++curToken;
    return node.release();
}

TreeNode *Syntax::Statement(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int &offset) {
    TreeNode *node = nullptr;
    switch (curToken->type) {
        case TokenType::KEY_BEGIN:  // 这里给个新的变量表
            node = Block(code, symtable, offset);
            break;
        case TokenType::ID:
            node = AssignStmt(code, symtable);
            break;
        case TokenType::KEY_RETURN:
            node = ReturnStmt(code, symtable);
            break;
        case TokenType::KEY_IF:
            node = IfStmt(code, symtable, offset);
            break;
        case TokenType::KEY_WRITE:
            node = WriteStmt(code, symtable);
            break;
        case TokenType::KEY_READ:
            node = ReadStmt(code, symtable);
            break;
        case TokenType::KEY_WHILE:
            node = WhileStmt(code, symtable, offset);
            break;
        case TokenType::KEY_REPEAT:
            node = RepeatStmt(code, symtable, offset);
            break;
        default:
            node = LocalVarDecl(symtable, offset, code);
            break;
    }
    return node;
}

// code可能没用到，只有是声明并初始化时才有用
TreeNode *Syntax::LocalVarDecl(unordered_map<string, Symbol> &symtable, unsigned int &offset, vector<string> &code) {
    unique_ptr<TreeNode> node(new TreeNode(NodeType::LocalVarDecl));
    // unordered_set<TokenType> declSet = {TokenType::KEY_INT, TokenType::KEY_STRING, TokenType::KEY_REAL};
    Symbol sym{offset};
    if (curToken->type == TokenType::KEY_INT) {
        sym.size = INT_SIZE;
        sym.type = Symbol_Type::INT;
    } else if (curToken->type == TokenType::KEY_REAL) {
        sym.size = REAL_SIZE;
        sym.type = Symbol_Type::REAL;
    } else if (curToken->type == TokenType::KEY_BOOLEN) {
        sym.size = BOOLEN_SIZE;
        sym.type = Symbol_Type::BOOLEN;
    } else if (curToken->type == TokenType::KEY_STRING) {
        sym.size = STRING_SIZE;
        sym.type = Symbol_Type::STRING;
    } else {
        throw Exception("Invalid value declare", curToken->row, curToken->column);
    }
    offset += sym.size;
    ++curToken;
    if (curToken->type != TokenType::ID) {
        throw Exception("Invalid value declare, Id expected", curToken->row, curToken->column);
    }
    if (symtable.find(curToken->value) != symtable.end()) {  // 重复定义
        throw Exception("Redefined", curToken->row, curToken->column);
    }
    string &id = curToken->value;
    node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
    ++curToken;
    if (curToken->type == TokenType::SYM_ASSIGN) {  // 声明并赋值
        ++curToken;
        string tmp;
        node->child.emplace_back(Expression(code, symtable, tmp, false));
        code.emplace_back(id + "=" + tmp);
    }
    symtable[id] = sym; // 放到最后才插入，考虑到int a=a+1是不合法的
    Semicolon();
    return node.release();
}

TreeNode *Syntax::AssignStmt(vector<string> &code, unordered_map<string, Symbol> &symtable) {
    if (symtable.find(curToken->value) == symtable.end()) {
        throw Exception("variable undefined", curToken->row, curToken->column);
    }
    unique_ptr<TreeNode> node(new TreeNode(NodeType::AssignStmt));
    node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));  // id
    string &id = curToken->value;
    ++curToken;
    if (curToken->type != TokenType::SYM_ASSIGN) {
        throw Exception("':=' expected", curToken->row, curToken->column);
    }
    ++curToken;
    string t;
    node->child.emplace_back(Expression(code, symtable, t, false));
    code.emplace_back(id + " = " + t);
    Semicolon();
    return node.release();
}

// 返回值放在%ax寄存器里
TreeNode *Syntax::ReturnStmt(vector<string> &code, unordered_map<string, Symbol> &symtable) {
    unique_ptr<TreeNode> node(new TreeNode(NodeType::ReturnStmt));
    ++curToken;
    string t;
    node->child.emplace_back(Expression(code, symtable, t, false));
    Semicolon();
    code.emplace_back("%ax = " + t);
    code.emplace_back("ret");
    return node.release();
}

TreeNode *Syntax::IfStmt(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int &offset) {
    unique_ptr<TreeNode> node(new TreeNode(NodeType::IfStmt));
    ++curToken;
    if (curToken->type != TokenType::SYM_LP) {
        throw Exception("'(' expected", curToken->row, curToken->column);
    }
    ++curToken;
    string cond;
    node->child.emplace_back(Expression(code, symtable, cond, true));
    // 此时code的最后两行是还没完整的
    // 倒数第二行条是true出口
    // 倒数第一条是false出口
    if (curToken->type != TokenType::SYM_RP) {
        throw Exception("')' expected", curToken->row, curToken->column);
    }
    ++curToken;
    unordered_map<string, Symbol> &tmpTable(symtable);
    vector<string> newCodes;
    node->child.emplace_back(Statement(newCodes, tmpTable, offset));
    string l1 = move(newLabel()), l2 = move(newLabel());  // l1是expression的true出口，l2是false出口
    code.at(code.size() - 2) += l1;
    code.back() += l2;
    code.emplace_back(l1 + ":");
    if (curToken->type == TokenType::KEY_ELSE) {
        ++curToken;
        unordered_map<string, Symbol> elseTable(symtable);
        vector<string> elseCode;
        node->child.emplace_back(Statement(elseCode, elseTable, offset));
        string l3 = move(newLabel());
        newCodes.emplace_back("goto " + l3);
        newCodes.emplace_back(l2 + ":");
        newCodes.insert(newCodes.end(), elseCode.begin(), elseCode.end());
        newCodes.emplace_back(l3 + ":");
    } else {
        newCodes.emplace_back(l2 + ":");
    }
    code.insert(code.end(), newCodes.begin(), newCodes.end());
    return node.release();
}

TreeNode *Syntax::WriteStmt(vector<string> &code, unordered_map<string, Symbol> &symtable) {
    unique_ptr<TreeNode> node(new TreeNode(NodeType::WriteStmt));
    ++curToken;
    if (curToken->type != TokenType::SYM_LP) {
        throw Exception("'(' expected", curToken->row, curToken->column);
    }
    ++curToken;
    string val;
    node->child.emplace_back(Expression(code, symtable, val, false));
    if (curToken->type != TokenType::SYM_COMMA) {
        throw Exception("',' expected", curToken->row, curToken->column);
    }
    ++curToken;
    if (curToken->type != TokenType::STR) {
        throw Exception("A string expected", curToken->row, curToken->column);
    }
    node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
    string &str = curToken->value;
    ++curToken;
    if (curToken->type != TokenType::SYM_RP) {
        throw Exception("')' expected", curToken->row, curToken->column);
    }
    ++curToken;
    Semicolon();
    code.emplace_back("write " + val + " " + str);
    return node.release();
}


TreeNode *Syntax::ReadStmt(vector<string> &code, unordered_map<string, Symbol> &symtable) {
    unique_ptr<TreeNode> node(new TreeNode(NodeType::ReadStmt));
    ++curToken;
    if (curToken->type != TokenType::SYM_LP) {
        throw Exception("'(' expected", curToken->row, curToken->column);
    }
    ++curToken;
    if (curToken->type != TokenType::ID) {
        throw Exception("ID expected", curToken->row, curToken->column);
    }
    if (symtable.find(curToken->value) == symtable.end()) {
        throw Exception("variable undefined", curToken->row, curToken->column);
    }
    string &id = curToken->value;
    node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));  // ID
    ++curToken;
    if (curToken->type != TokenType::SYM_COMMA) {
        throw Exception("',' expected", curToken->row, curToken->column);
    }
    ++curToken;
    if (curToken->type != TokenType::STR) {
        throw Exception("A string expected", curToken->row, curToken->column);
    }
    node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));  // String
    string val = curToken->value;
    ++curToken;
    if (curToken->type != TokenType::SYM_RP) {
        throw Exception("')' expected", curToken->row, curToken->column);
    }
    ++curToken;
    Semicolon();
    code.emplace_back("read " + id + " " + val);
    return node.release();
}

TreeNode *Syntax::WhileStmt(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int &offset) {
    unique_ptr<TreeNode> node(new TreeNode(NodeType::WhileStmt));
    ++curToken;
    if (curToken->type != TokenType::SYM_LP) {
        throw Exception("'(' expected", curToken->row, curToken->column);
    }
    ++curToken;
    vector<string> condCodes;
    string cond;
    node->child.emplace_back(Expression(condCodes, symtable, cond, true));
    if (curToken->type != TokenType::SYM_RP) {
        throw Exception("')' expected", curToken->row, curToken->column);
    }
    ++curToken;
    vector<string> whileCodes;
    unordered_map<string, Symbol> tmpTable(symtable);
    node->child.emplace_back(Statement(whileCodes, tmpTable, offset));
    string l1 = move(newLabel()), l2 = move(newLabel()), l3 = move(newLabel());
    condCodes.at(condCodes.size() - 2) += l1;
    condCodes.back() += l2;
    condCodes.emplace_back(l1 + ":");
    code.emplace_back(l3 + ":");
    code.insert(code.end(), condCodes.begin(), condCodes.end());
    code.insert(code.end(), whileCodes.begin(), whileCodes.end());
    code.emplace_back("goto " + l3);
    code.emplace_back(l2 + ":");
    return node.release();
}

TreeNode *Syntax::RepeatStmt(vector<string> &code, unordered_map<string, Symbol> &symtable, unsigned int &offset) {
    unique_ptr<TreeNode> node(new TreeNode(NodeType::RepeatStmt));

    ++curToken;
    vector<string> repeatCode;
    unordered_map<string, Symbol> repeatTable(symtable);
    node->child.emplace_back(Statement(repeatCode, repeatTable, offset));
    if (curToken->type != TokenType::KEY_UNTIL) {
        throw Exception("UNTILE expected", curToken->row, curToken->column);
    }
    ++curToken;
    if (curToken->type != TokenType::SYM_LP) {
        throw Exception("'(' expected", curToken->row, curToken->column);
    }
    ++curToken;
    string cond;
    vector<string> condCode;
    node->child.emplace_back(Expression(condCode, symtable, cond, true));
    condCode.pop_back();  // 最后一条跳转不需要了
    if (curToken->type != TokenType::SYM_RP) {
        throw Exception("')' expected", curToken->row, curToken->column);
    }
    string l1 = move(newLabel());
    condCode.back() += l1;
    code.emplace_back(l1 + ":");
    code.insert(code.end(), repeatCode.begin(), repeatCode.end());
    code.insert(code.end(), condCode.begin(), condCode.end());
    return node.release();
}

void Syntax::Semicolon() {
    if (curToken->type != TokenType::SYM_SEMICOLON) {
        throw Exception("';' expected", curToken->row, curToken->column);
    }
    ++curToken;
}


// OR表达式
TreeNode *Syntax::Expression(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res, bool control) {
    string t1, t2;
    unique_ptr<TreeNode> left(BoolTerm(code, symtable, t1, control)), right, node;
    if (curToken->type == TokenType::KEY_OR) {
        ++curToken;
        node.reset(new TreeNode(NodeType::LOG_OR_EXP));
        vector<string> newCodes;
        right.reset(Expression(newCodes, symtable, t2, control));
        if (!right) {
            throw Exception("Invalid input", curToken->row, curToken->column);
        }
        string l1 = move(newLabel()), l2 = move(newLabel());
        if (!control) {
            if (left->type == NodeType::Terminator) {
                res = move(newTemp());
                code.emplace_back(res + " = " + t1);
            } else {
                // t1=%t13
                res = t1;
            }
            code.emplace_back("if " + t1 + " goto " + l1);
            newCodes.emplace_back(res + " = " + t2);
            newCodes.emplace_back(l1 + ":");
        } else {  // 此时最后一条的目标地址还没填
            newCodes.at(newCodes.size() - 2) += l1;  // if t2 goto l1
            newCodes.back() += l2; // goto l2
            // true出口在上，false出口在下，做到和and统一
            newCodes.emplace_back(l1 + ": goto "); // true出口
            newCodes.emplace_back(l2 + ": goto "); // false出口

            code.pop_back();
            code.back() += l1;
        }
        node->child.emplace_back(left.release());
        node->child.emplace_back(right.release());
        code.insert(code.end(), newCodes.begin(), newCodes.end());
        return node.release();
    }
    res = t1;  // 如果是control，res好像就没用了
    return left.release();
}

// and表达式
TreeNode *Syntax::BoolTerm(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res, bool control) {
    string t1, t2;
    unique_ptr<TreeNode> left(BoolFactor(code, symtable, t1)), right, node;
    if (!left) {
        throw Exception("Invalid input", curToken->row, curToken->column);
    }
    if (curToken->type == TokenType::KEY_AND) {
        node.reset(new TreeNode(NodeType::LOG_AND_EXP));
        ++curToken;
        vector<string> newCodes;
        right.reset(BoolTerm(newCodes, symtable, t2, control));
        if (!right) {
            throw Exception("Invalid input", curToken->row, curToken->column);
        }
        string l1 = move(newLabel()), l2 = move(newLabel());
        if (!control) {  // 引入短路机制
            if (left->type == NodeType::Terminator) {
                res = move(newTemp());
                code.emplace_back(res + " = " + t1);
            } else {
                res = t1;
            }
            // 不管t2是不是变量
            newCodes.emplace_back(l1 + ": " + res + " = " + t2);
            newCodes.emplace_back(l2 + ":");
        } else {  // 此时newCodes最后两行代码还没完整
            newCodes.front() = l1 + ": " + newCodes.front();
            newCodes.back() = l2 + ": " + newCodes.back();  // newCodes.back() 是 goto _
        }
        code.emplace_back("if " + t1 + " goto " + l1);
        code.emplace_back("goto " + l2);
        node->child.emplace_back(left.release());
        node->child.emplace_back(right.release());
        code.insert(code.end(), newCodes.begin(), newCodes.end());
        return node.release();
    }
    if (control) {
        code.emplace_back("if " + t1 + " goto ");  // 这行代码还没写完
        code.emplace_back("goto ");
    }
    res = t1;
    return left.release();
}

// 这里只有一个综合属性res就可以了
TreeNode *Syntax::BoolFactor(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res) {
    // string或者arithmetic的比较，或true或者false，或者只有一个string或arithmetic
    // TreeNode *left = nullptr, *right = nullptr;
    unique_ptr<TreeNode> node, left, right;
    vector<Token>::iterator tmpToken = curToken;
    string t1, t2;
    if (curToken->type == TokenType::STR) {
        left.reset(StringExpression(t1));
    } else if (curToken->type == TokenType::KEY_TRUE || curToken->type == TokenType::KEY_FALSE) {
        left.reset(new TreeNode(NodeType::Terminator, *curToken));
        if (curToken->type == TokenType::KEY_TRUE) {
            t1 = "ture";
        } else {
            t1 = "false";
        }
        ++curToken;
    } else {
        left.reset(ArithmeticExpression(code, symtable, t1));
    }

    if (left == nullptr) {  // 回溯
        curToken = tmpToken;
        if (curToken->type == TokenType::SYM_LP) {
            ++curToken;
            left.reset(BoolFactor(code, symtable, t1));  // 递归分析
            if (curToken->type != TokenType::SYM_RP) {
                throw Exception("')' expected", curToken->row, curToken->column);
            }
            if (left == nullptr) {
                throw Exception("Invalid input ()", curToken->row, curToken->column);
            }
            ++curToken;
        } else {
            throw Exception("Invalid input", curToken->row, curToken->column);
        }
    }
    unordered_set<TokenType> compSet = {TokenType::SYM_EQU, TokenType::SYM_GEQ, TokenType::SYM_LEQ, TokenType::SYM_GT,
                                        TokenType::SYM_LT, TokenType::SYM_NEQU};
    if (match(compSet)) {
        res = move(newTemp());
        string newCode = res + " = " + t1;
        if (curToken->type == TokenType::SYM_EQU) {
            newCode += " == ";
            node.reset(new TreeNode(NodeType::EQU_EXP));
        } else if (curToken->type == TokenType::SYM_LT) {
            newCode += " < ";
            node.reset(new TreeNode(NodeType::LES_EXP));
        } else if (curToken->type == TokenType::SYM_GT) {
            newCode += " > ";
            node.reset(new TreeNode(NodeType::GTR_EXP));
        } else if (curToken->type == TokenType::SYM_LEQ) {
            newCode += " <= ";
            node.reset(new TreeNode(NodeType::LEQ_EXP));
        } else if (curToken->type == TokenType::SYM_GEQ) {
            newCode += " >= ";
            node.reset(new TreeNode(NodeType::GEQ_EXP));
        } else if (curToken->type == TokenType::SYM_NEQU) {
            newCode += " != ";
            node.reset(new TreeNode(NodeType::NEQU_EXP));
        }
        ++curToken;
        right.reset(BoolFactor(code, symtable, t2));
        if (right == nullptr) {
            throw Exception("Invalid input", curToken->row, curToken->column);
        }
        newCode += t2;
        code.emplace_back(newCode);
        node->child.emplace_back(left.release());
        node->child.emplace_back(right.release());
        return node.release();
    }
    res = t1;
    return left.release();
}


TreeNode *Syntax::ArithmeticExpression(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res) {
    unique_ptr<TreeNode> node;
    if (curToken->type == TokenType::SYM_SUB) {  // Arithmetic -> - Arithmetic
        node.reset(new TreeNode(NodeType::OPP_EXP));
        ++curToken;
        string tmp;
        node->child.emplace_back(ArithmeticExpression(code, symtable, tmp));
        res = move(newTemp());
        code.emplace_back(res + "= - " + tmp);
        return node.release();
    }
    string t1, t2;
    unique_ptr<TreeNode> left(MultiplicativeExpr(code, symtable, t1)), right;
    if (!left) {  // left为空
        return nullptr;
    }
    if (curToken->type == TokenType::SYM_ADD || curToken->type == TokenType::SYM_SUB) {
        res = move(newTemp());
        string newCode = res + " = " + t1;
        if (curToken->type == TokenType::SYM_SUB) {
            newCode += " - ";
            node.reset(new TreeNode(NodeType::SUB_EXP));
        } else {
            newCode += " + ";
            node.reset(new TreeNode(NodeType::ADD_EXP));
        }
        ++curToken;
        right.reset(ArithmeticExpression(code, symtable, t2));  // 递归处理a1+a2+a3..
        if (!right) { // right为空
            throw Exception("Invalid input", curToken->row, curToken->column);
        }
        newCode += t2;
        code.emplace_back(newCode);
        node->child.emplace_back(left.release());
        node->child.emplace_back(right.release());
        return node.release();
    }
    res = t1;
    return left.release();
}

TreeNode *Syntax::MultiplicativeExpr(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res) {
    unique_ptr<TreeNode> left, right;
    unique_ptr<TreeNode> node;
    // string t1 = curToken->value;  // 无论是id还是num
    string t1, t2;
    if (curToken->type == TokenType::NUM) {
        t1 = curToken->value;
        left.reset(new TreeNode(NodeType::Terminator, *curToken));
        ++curToken;
    } else if (curToken->type == TokenType::ID) {
        // 这时有两种可能，一个是 MultiplicativeExpr -> Id 还有就是 MultiplicativeExpr -> PrimaryExpr
        // 向前看一个
        vector<Token>::iterator tmpToken = curToken;
        ++curToken;
        if (curToken->type != TokenType::SYM_LP) {  // MultiplicativeExpr -> Id
            if (symtable.find(tmpToken->value) == symtable.end()) {  // 使用了未定义的id
                throw Exception("variable undefined", tmpToken->row, tmpToken->column);
            }
            t1 = tmpToken->value;
            left.reset(new TreeNode(NodeType::Terminator, *tmpToken));
        } else {
            curToken = tmpToken;  // MultiplicativeExpr -> PrimaryExpr
            TreeNode *tmp = PrimaryExpr(code, symtable, t1);
            if (tmp == nullptr) {
                return nullptr;
            }
            left.reset(tmp);
        }
    } else {
        if (curToken->type == TokenType::SYM_LP) { // MultiplicativeExpr -> PrimaryExpr
            TreeNode *tmp = PrimaryExpr(code, symtable, t1);
            if (tmp == nullptr) {
                return nullptr;
            }
            // node->child.emplace_back(tmp);
            left.reset(tmp);
        } else {
            throw Exception("'(' expected", curToken->row, curToken->column);
        }
    }
    if (curToken->type == TokenType::SYM_MUL || curToken->type == TokenType::SYM_DIV) {
        res = move(newTemp());
        string newCode = res + " = " + t1;
        if (curToken->type == TokenType::SYM_DIV) {
            node.reset(new TreeNode(NodeType::DIV_EXP));
            newCode += " / ";
        } else {
            node.reset(new TreeNode(NodeType::MUL_EXP));
            newCode += " * ";
        }
        ++curToken;
        right.reset(MultiplicativeExpr(code, symtable, t2));  // 递归实现多个乘除
        if (!right) {
            throw Exception("Invalid input", curToken->row, curToken->column);
        }
        node->child.emplace_back(left.release());
        node->child.emplace_back(right.release());
        newCode += t2;
        code.emplace_back(newCode);
        return node.release();
    }
    res = t1;
    return left.release();
}

TreeNode *Syntax::PrimaryExpr(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res) {
    unique_ptr<TreeNode> node;
    if (curToken->type == TokenType::SYM_LP) {  // 遇到左括号，使用 PrimaryExpr -> ( ArthmeticExpression )
        ++curToken;
        node.reset(ArithmeticExpression(code, symtable, res));
        if (!node) {  // 应该回溯
            return nullptr;
        }
        if (curToken->type != TokenType::SYM_RP) {  // 可能是比较符号，应该回溯
            unordered_set<TokenType> compSet = {TokenType::SYM_EQU, TokenType::SYM_GEQ, TokenType::SYM_LEQ,
                                                TokenType::SYM_GT, TokenType::SYM_LT, TokenType::SYM_NEQU};
            if (match(compSet)) {  // 回溯
                node.reset();
                return nullptr;
            }
            throw Exception("')' expected", curToken->row, curToken->column);
        }
        ++curToken;
        return node.release();
    }
    // 初步设想就是参考c++，使用一个寄存器返回，即%ax
    // a=f(p1,p2,p3) 生成的代码为
    // ...
    // call f
    // a = %ax
    if (curToken->type != TokenType::ID) {
        throw Exception("Id expected", curToken->row, curToken->column);
    }
    string fun = curToken->value;
    if (funTable.find(fun) == funTable.end()) {
        throw Exception("method undefined", curToken->row, curToken->column);
    }
    node.reset(new TreeNode(NodeType::PrimaryExpr, *curToken));
    // node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
    ++curToken;
    if (curToken->type != TokenType::SYM_LP) {
        throw Exception("'(' expected", curToken->row, curToken->column);
    }
    ++curToken;
    vector<string> params; // 参数
    while (curToken->type != TokenType::SYM_RP) {
        string p;
        node->child.emplace_back(ActualParams(code, symtable, p));
        params.emplace_back(move(p));
    }
    ++curToken;
    for (auto &p:params) {
        code.emplace_back("param " + p);
    }
    code.emplace_back("call " + fun);
    // 当 t = f1() + f2()
    // %ax会被占用，所以要产生一个新的临时变量保存返回值
    res = move(newTemp());
    code.emplace_back(res + " = %ax");
    return node.release();
}

// ActualParams应该不用返回值res, 只需要生成代码即可
// 或者ActualParams有返回值res，每次调用就分析一个参数并且生成代码
TreeNode *Syntax::ActualParams(vector<string> &code, unordered_map<string, Symbol> &symtable, string &res) {
//    unique_ptr<TreeNode> node(new TreeNode(NodeType::ActualParams));
//    node->child.emplace_back(Expression());
    unique_ptr<TreeNode> node(Expression(code, symtable, res, false));
    if (!node) {
        throw Exception("invalid parameter", curToken->row, curToken->column);
    }
    if (curToken->type == TokenType::SYM_COMMA) {
        ++curToken;
    }
//    while (curToken->type == TokenType::SYM_COMMA) {
//        node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
//        ++curToken;
//        node->child.emplace_back(Expression());
//    }
    return node.release();
}

TreeNode *Syntax::StringExpression(string &res) {
    TreeNode *node = new TreeNode(NodeType::Terminator, *curToken);
    res = curToken->value;
    ++curToken;
    return node;
}

TreeNode *Syntax::Program() {
    unique_ptr<TreeNode> node(new TreeNode(NodeType::Program));
    unordered_set<TokenType> matchSet = {TokenType::KEY_INT, TokenType::KEY_REAL, TokenType::KEY_STRING,
                                         TokenType::KEY_BOOLEN};
    if (!match(matchSet)) {
        throw Exception("Invalid input", curToken->row, curToken->column);
    }
    node->child.emplace_back(MethodDecl(interCode, globalSym, symOffset));
    if (curToken->type != TokenType::END) {  // P -> MD P
        node->child.emplace_back(Program());  // 递归调用
    }
    return node.release();
}

string Syntax::newTemp() {
    string t = "%t" + to_string(tmpCount);
    ++tmpCount;
    return t;
}

string Syntax::newLabel() {
    string l = "@L" + to_string(labelCount);
    ++labelCount;
    return l;
}

void Syntax::writeCode() {
    for (auto &c:interCode) {
        out << c << endl;
    }
}
