#include "syntax/syntax.h"
#include"exceptions.h"
#include<iostream>
#include<iomanip>
#include<stack>
#include<memory>

using namespace std;

Syntax::Syntax(Lex *lex) throw(Exception) :root(nullptr) {
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
	if (root != nullptr) {
		delete root;
	}
}


// 打印一颗子树
void Syntax::displayIntree(TreeNode *root, int depth) const {
	cout << setw(15) << left << root->getType();
	if (root->type == NodeType::Terminator) {
		cout << left << root->token.value;
	}
	if (root->child.empty()) {
		cout << endl;
	} else {
		displayIntree(root->child.front(), depth + 1);  // 打印第一个子节点
		list<TreeNode *>::const_iterator citer = root->child.cbegin();
		for (++citer; citer != root->child.cend(); ++citer) {  // 打印其余的子节点
			for (int i = 0; i < depth + 1; ++i) {
				cout << "               ";
			}
			displayIntree(*citer, depth + 1);
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
void Syntax::program() {
	unique_ptr<TreeNode> unique_root(new TreeNode(NodeType::Program));
	unordered_set<TokenType> matchSet = { TokenType::KEY_INT,TokenType::KEY_REAL,TokenType::KEY_STRING };
	while (curToken->type != TokenType::END) {
		if (match(matchSet)) {
			unique_root->child.emplace_back(MethodDecl());
		} else {
			throw Exception("Invalid Input", curToken->row, curToken->column);
		}
	}
	root = unique_root.release();
}

void Syntax::display()const {
	displayIntree(root, 0);
}

TreeNode *Syntax::MethodDecl() {
	unique_ptr<TreeNode> node(new TreeNode(NodeType::MethodDecl));
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	if (curToken->type == TokenType::ID || curToken->type == TokenType::KEY_MAIN) {
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	} else {
		throw Exception("ID or MAIN expected", curToken->row, curToken->column);
	}
	++curToken;
	if (curToken->type == TokenType::SYM_LP) {
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	} else {
		throw Exception("'(' expected", curToken->row, curToken->column);
	}
	++curToken;
	if (curToken->type == TokenType::SYM_RP) {
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
		++curToken;
	} else {
		node->child.emplace_back(FormalParams());
		if (curToken->type != TokenType::SYM_RP) {
			throw Exception("')' expected", curToken->row, curToken->column);
		}
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
		++curToken;
	}
	if (curToken->type != TokenType::KEY_BEGIN) {
		throw Exception("BEGIN expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(Block());
	return node.release();
}

TreeNode *Syntax::FormalParams() {
	unordered_set<TokenType> typeSet = { TokenType::KEY_INT,TokenType::KEY_REAL,TokenType::KEY_STRING };
	if (!match(typeSet)) {
		throw Exception("Invalid parameter", curToken->row, curToken->column);
	}
	unique_ptr<TreeNode> node(new TreeNode(NodeType::FormalParams));
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	if (curToken->type != TokenType::ID) {
		throw Exception("ID expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	while (curToken->type == TokenType::SYM_COMMA) {
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));  // ,
		++curToken;
		if (!match(typeSet)) {
			throw Exception("Invalid parameter", curToken->row, curToken->column);
		}
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));  // Type
		++curToken;
		if (curToken->type != TokenType::ID) {
			throw Exception("Invalid parameter, id expected", curToken->row, curToken->column);
		}
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));  // ID
		++curToken;
	}
	return node.release();
}

TreeNode *Syntax::Block() {
	unique_ptr<TreeNode> node(new TreeNode(NodeType::Block));
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken)); // BEGIN关键字
	++curToken;
	unordered_set<TokenType> stmtSet = { TokenType::KEY_BEGIN,TokenType::KEY_INT,TokenType::KEY_STRING,
		TokenType::KEY_REAL,TokenType::ID,TokenType::KEY_RETURN,TokenType::KEY_IF,TokenType::KEY_WRITE,
		TokenType::KEY_READ,TokenType::KEY_WHILE,TokenType::KEY_REPEAT };
	while (match(stmtSet)) {
		node->child.emplace_back(Statement());
	}
	if (curToken->type != TokenType::KEY_END) {
		throw Exception("END expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	return node.release();
}

TreeNode *Syntax::Statement() {
	TreeNode *node = nullptr;
	switch (curToken->type) {
	case TokenType::KEY_BEGIN:
		node = Block();
		break;
	case TokenType::ID:
		node = AssignStmt();
		break;
	case TokenType::KEY_RETURN:
		node = ReturnStmt();
		break;
	case TokenType::KEY_IF:
		node = IfStmt();
		break;
	case TokenType::KEY_WRITE:
		node = WriteStmt();
		break;
	case TokenType::KEY_READ:
		node = ReadStmt();
		break;
	case TokenType::KEY_WHILE:
		node = WhileStmt();
		break;
	case TokenType::KEY_REPEAT:
		node = RepeatStmt();
		break;
	default:
		node = LocalVarDecl();
		break;
	}
	return node;
}

TreeNode *Syntax::LocalVarDecl() {
	unique_ptr<TreeNode> node(new TreeNode(NodeType::LocalVarDecl));
	unordered_set<TokenType> declSet = { TokenType::KEY_INT,TokenType::KEY_STRING,TokenType::KEY_REAL };
	if (match(declSet)) {
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
		++curToken;
	} else {
		throw Exception("Invalid value declare", curToken->row, curToken->column);
	}
	if (curToken->type != TokenType::ID) {
		throw Exception("Invalid value declare, Id expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	if (curToken->type == TokenType::SYM_ASSIGN) {  // 声明并赋值
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
		++curToken;
		node->child.emplace_back(Expression());
	}
	node->child.emplace_back(Semicolon());
	return node.release();
}

TreeNode *Syntax::AssignStmt() {
	unique_ptr<TreeNode> node(new TreeNode(NodeType::AssignStmt));
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	if (curToken->type != TokenType::SYM_ASSIGN) {
		throw Exception("'=' expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	node->child.emplace_back(Expression());
	node->child.emplace_back(Semicolon());
	return node.release();
}

TreeNode *Syntax::ReturnStmt() {
	unique_ptr<TreeNode> node(new TreeNode(NodeType::ReturnStmt));
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	node->child.emplace_back(Expression());
	node->child.emplace_back(Semicolon());
	return node.release();
}

TreeNode *Syntax::IfStmt() {
	unique_ptr<TreeNode> node(new TreeNode(NodeType::IfStmt));
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	if (curToken->type != TokenType::SYM_LP) {
		throw Exception("'(' expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	node->child.emplace_back(Expression());
	if (curToken->type != TokenType::SYM_RP) {
		throw Exception("')' expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	node->child.emplace_back(Statement());
	if (curToken->type == TokenType::KEY_ELSE) {
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
		++curToken;
		node->child.emplace_back(Statement());
	}
	return node.release();
}

TreeNode *Syntax::WriteStmt() {
	unique_ptr<TreeNode> node(new TreeNode(NodeType::WriteStmt));
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	if (curToken->type != TokenType::SYM_LP) {
		throw Exception("'(' expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	node->child.emplace_back(Expression());
	if (curToken->type != TokenType::SYM_COMMA) {
		throw Exception("',' expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	if (curToken->type != TokenType::STR) {
		throw Exception("A string expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	if (curToken->type != TokenType::SYM_RP) {
		throw Exception("')' expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	node->child.emplace_back(Semicolon());
	return node.release();
}


TreeNode *Syntax::ReadStmt() {
	unique_ptr<TreeNode> node(new TreeNode(NodeType::ReadStmt));
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));  // READ
	++curToken;
	if (curToken->type != TokenType::SYM_LP) {
		throw Exception("'(' expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));  // (
	++curToken;
	if (curToken->type != TokenType::ID) {
		throw Exception("ID expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));  // ID
	++curToken;
	if (curToken->type != TokenType::SYM_COMMA) {
		throw Exception("',' expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));  // ,
	++curToken;
	if (curToken->type != TokenType::STR) {
		throw Exception("A string expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));  // String
	++curToken;
	if (curToken->type != TokenType::SYM_RP) {
		throw Exception("')' expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken)); // )
	++curToken;
	node->child.emplace_back(Semicolon());
	return node.release();
}

TreeNode *Syntax::WhileStmt() {

	unique_ptr<TreeNode> node(new TreeNode(NodeType::WhileStmt));
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	if (curToken->type != TokenType::SYM_LP) {
		throw Exception("'(' expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	node->child.emplace_back(Expression());
	if (curToken->type != TokenType::SYM_RP) {
		throw Exception("')' expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	node->child.emplace_back(Statement());
	return node.release();
}

TreeNode *Syntax::RepeatStmt() {
	unique_ptr<TreeNode> node(new TreeNode(NodeType::RepeatStmt));
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	node->child.emplace_back(Statement());
	if (curToken->type != TokenType::KEY_UNTIL) {
		throw Exception("UNTILE expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	if (curToken->type != TokenType::SYM_LP) {
		throw Exception("'(' expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	node->child.emplace_back(Expression());
	if (curToken->type != TokenType::SYM_RP) {
		throw Exception("')' expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	return node.release();
}

TreeNode *Syntax::Semicolon() {
	if (curToken->type != TokenType::SYM_SEMICOLON) {
		throw Exception("';' expected", curToken->row, curToken->column);
	}
	TreeNode *node = new TreeNode(NodeType::Terminator, *curToken);
	++curToken;
	return node;
}

TreeNode *Syntax::Expression() {
	TreeNode *left = BoolTerm(), *right = nullptr, *node = nullptr;
	if (curToken->type == TokenType::KEY_AND) {
		vector<Token>::iterator tmpToken = curToken;
		++curToken;
		right = Expression();
		node = new TreeNode(NodeType::LOG_OR_EXP);
		node->child.emplace_back(left);
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *tmpToken));
		node->child.emplace_back(right);
	}
	if (node != nullptr) {
		return node;
	}
	return left;
}

TreeNode *Syntax::BoolTerm() {
	TreeNode *left = BoolFactor(), *right = nullptr, *node = nullptr;
	if (left == nullptr) {
		throw Exception("Invalid input", curToken->row, curToken->column);
	}
	if (curToken->type == TokenType::KEY_AND) {
		vector<Token>::iterator tmpToken = curToken;
		++curToken;
		right = BoolTerm();
		node = new TreeNode(NodeType::LOG_AND_EXP);
		node->child.emplace_back(left);
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *tmpToken));
		node->child.emplace_back(right);
	}
	if (node != nullptr) {
		return node;
	}
	return left;
}

TreeNode *Syntax::BoolFactor() {
	TreeNode *left = nullptr, *right = nullptr;
	unique_ptr<TreeNode> node;
	vector<Token>::iterator tmpToken = curToken;
	if (curToken->type == TokenType::STR) {
		left = StringExpression();
	} else if (curToken->type == TokenType::KEY_TRUE || curToken->type == TokenType::KEY_FALSE) {
		left = new TreeNode(NodeType::Terminator, *curToken);
		++curToken;
	}
	else {
		left = ArithmeticExpression();
	}

	if (left == nullptr) {  // 回溯
		curToken = tmpToken;
		if (curToken->type == TokenType::SYM_LP) {
			++curToken;
			left = BoolFactor();  // 递归分析
			if (curToken->type != TokenType::SYM_RP) {
				throw Exception("')' expected", curToken->row, curToken->column);
			}
			if (left == nullptr) {
				throw Exception("Invalid input ()", curToken->row, curToken->column);
			}
			node.reset(new TreeNode(NodeType::EQU_EXP));
			node->child.emplace_back(new TreeNode(NodeType::Terminator, *tmpToken));  // (
			node->child.emplace_back(left);
			node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));  // )
			node->type = left->type;
			++curToken;
		} else {
			throw Exception("Invalid input", curToken->row, curToken->column);
		}
	}
	unordered_set<TokenType> compSet = { TokenType::SYM_EQU,TokenType::SYM_GEQ,TokenType::SYM_LEQ,TokenType::SYM_GT,TokenType::SYM_LT,TokenType::SYM_NEQU };
	if (match(compSet)) {
		tmpToken = curToken;
		++curToken;
		right = BoolFactor();
		if (right == nullptr) {
			throw Exception("Invalid input", curToken->row, curToken->column);
		}
		if (!node) {
			node.reset(new TreeNode(NodeType::EQU_EXP));
			node->child.emplace_back(left);
		}
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *tmpToken));
		node->child.emplace_back(right);
		if (tmpToken->type == TokenType::SYM_EQU) {
			node->type = NodeType::EQU_EXP;
		} else if (tmpToken->type == TokenType::SYM_LT) {
			node->type = NodeType::LES_EXP;
		} else if (tmpToken->type == TokenType::SYM_GT) {
			node->type = NodeType::GTR_EXP;
		} else if (tmpToken->type == TokenType::SYM_LEQ) {
			node->type = NodeType::LEQ_EXP;
		} else if (tmpToken->type == TokenType::SYM_GEQ) {
			node->type = NodeType::GEQ_EXP;
		} else if (tmpToken->type == TokenType::SYM_NEQU) {
			node->type = NodeType::NEQU_EXP;
		} else {
			node->type = NodeType::EQU_EXP;
		}
	}
	if (node) {
		return node.release();
	}
	return left;
}


TreeNode *Syntax::ArithmeticExpression() {
	TreeNode *left = MultiplicativeExpr(), *right = nullptr;
	if (left == nullptr) {
		return nullptr;
	}
	//unique_ptr<TreeNode> node(new TreeNode(NodeType::ADD_EXP));
	//node->child.emplace_back(left);
	unique_ptr<TreeNode> node;
	if (curToken->type == TokenType::SYM_ADD || curToken->type == TokenType::SYM_SUB) {
		if (curToken->type == TokenType::SYM_SUB) {
			// node->type = NodeType::SUB_EXP;
			node.reset(new TreeNode(NodeType::SUB_EXP));
		} else {
			node.reset(new TreeNode(NodeType::ADD_EXP));
		}
		node->child.emplace_back(left);
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
		++curToken;
		right = MultiplicativeExpr();
		if (right == nullptr) {
			throw Exception("Invalid input", curToken->row, curToken->column);
		}
		node->child.emplace_back(right);
	}
	if (node) {
		return node.release();
	}
	return left;
}

TreeNode *Syntax::MultiplicativeExpr() {
	unique_ptr<TreeNode> left, right;
	unique_ptr<TreeNode> node;
	if (curToken->type == TokenType::NUM) {
		left.reset(new TreeNode(NodeType::Terminator, *curToken));
		++curToken;
	} else if (curToken->type == TokenType::ID) {  // 这时有两种可能，一个是 MultiplicativeExpr -> Id 还有就是 MultiplicativeExpr -> PrimaryExpr
		// 向前看一个
		vector<Token>::iterator tmpToken = curToken;
		++curToken;
		if (curToken->type != TokenType::SYM_LP) {  // MultiplicativeExpr -> Id
			left.reset(new TreeNode(NodeType::Terminator, *tmpToken));
			// ++curToken;
		} else {
			curToken = tmpToken;  // MultiplicativeExpr -> PrimaryExpr
			TreeNode *tmp = PrimaryExpr();
			if (tmp == nullptr) {
				return nullptr;
			}
			left.reset(tmp);
		}
	} else {  
		if (curToken->type == TokenType::SYM_LP) { // MultiplicativeExpr -> PrimaryExpr
			TreeNode *tmp = PrimaryExpr();
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
		if (curToken->type == TokenType::SYM_DIV) {
			node.reset(new TreeNode(NodeType::DIV_EXP));
		} else {
			node.reset(new TreeNode(NodeType::MUL_EXP));
		}
		node->child.emplace_back(left.release());
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
		++curToken;
		TreeNode *tmp = MultiplicativeExpr();  // 递归实现多个乘除
		if (tmp == nullptr) {
			throw Exception("Invalid input", curToken->row, curToken->column);
		}
		node->child.emplace_back(tmp);
	}
	if (node) {
		return node.release();
	}
	return left.release();
}

TreeNode *Syntax::PrimaryExpr() {
	unique_ptr<TreeNode> node(new TreeNode(NodeType::PrimaryExpr));
	if (curToken->type == TokenType::SYM_LP) {  // 遇到左括号，使用 PrimaryExpr -> ( ArthmeticExpression )
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
		++curToken;
		TreeNode *tmp = ArithmeticExpression();
		if (tmp == nullptr) {  // 应该回溯
			node.reset();
			return nullptr;
		}
		node->child.emplace_back(tmp);
		if (curToken->type != TokenType::SYM_RP) {  // 可能是比较符号，应该回溯
			unordered_set<TokenType> compSet = { TokenType::SYM_EQU,TokenType::SYM_GEQ,TokenType::SYM_LEQ,TokenType::SYM_GT,TokenType::SYM_LT ,TokenType::SYM_NEQU };
			if (match(compSet)) {  // 回溯
				node.reset();
				return nullptr;
			}
			throw Exception("')' expected", curToken->row, curToken->column);
		}
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
		++curToken;
		return node.release();
	}
	if (curToken->type != TokenType::ID) {
		throw Exception("Id expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	if (curToken->type != TokenType::SYM_LP) {
		throw Exception("'(' expected", curToken->row, curToken->column);
	}
	node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
	++curToken;
	if (curToken->type == TokenType::SYM_RP) {  // PrimaryExpr -> Id ( )
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
		++curToken;
	} else { // PrimaryExpr -> Id (ActualParams)
		node->child.emplace_back(ActualParams());
		if (curToken->type != TokenType::SYM_RP) {
			throw Exception("')' expected", curToken->row, curToken->column);
		}
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
		++curToken;
	}
	return node.release();
}

TreeNode *Syntax::ActualParams() {
	unique_ptr<TreeNode> node(new TreeNode(NodeType::ActualParams));
	node->child.emplace_back(Expression());
	while (curToken->type == TokenType::SYM_COMMA) {
		node->child.emplace_back(new TreeNode(NodeType::Terminator, *curToken));
		++curToken;
		node->child.emplace_back(Expression());
	}
	return node.release();
}

TreeNode *Syntax::StringExpression() {
	TreeNode *node = new TreeNode(NodeType::Terminator, *curToken);
	++curToken;
	return node;
}
