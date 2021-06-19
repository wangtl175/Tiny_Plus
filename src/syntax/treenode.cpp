#include"syntax/treenode.h"

const char *TreeNode::getType() const {
    switch (type) {
        case NodeType::Program:
            return "Program";
        case NodeType::MethodDecl:
            return "MethodDecl";
        case NodeType::FormalParams:
            return "FormalParams";
        case NodeType::Block:
            return "Block";
        case NodeType::LocalVarDecl:
            return "LocalVarDecl";
        case NodeType::AssignStmt:
            return "AssignStmt";
        case NodeType::ReturnStmt:
            return "ReturnStmt";
        case NodeType::IfStmt:
            return "IfStmt";
        case NodeType::ReadStmt:
            return "ReadStmt";
        case NodeType::WriteStmt:
            return "WriteStmt";
        case NodeType::WhileStmt:
            return "WhileStmt";
        case NodeType::RepeatStmt:
            return "RepeatStmt";
        case NodeType::PrimaryExpr:
            return "PrimaryExpr";
        case NodeType::ActualParams:
            return "ActualParams";
        case NodeType::GTR_EXP:
            return "GTR_EXP";
        case NodeType::GEQ_EXP:
            return "GEQ_EXP";
        case NodeType::LES_EXP:
            return "LES_EXP";
        case NodeType::LEQ_EXP:
            return "LEQ_EXP";
        case NodeType::EQU_EXP:
            return "EQU_EXP";
        case NodeType::NEQU_EXP:
            return "NEQU_EXP";
        case NodeType::LOG_OR_EXP:
            return "LOG_OR_EXP";
        case NodeType::LOG_AND_EXP:
            return "LOG_AND_EXP";
        case NodeType::ADD_EXP:
            return "ADD_EXP";
        case NodeType::SUB_EXP:
            return "SUB_EXP";
        case NodeType::MUL_EXP:
            return "MUL_EXP";
        case NodeType::DIV_EXP:
            return "DIV_EXP";
        case NodeType::Terminator:
            return "Terminator";
        default:
            return "";
    }
}

TreeNode::TreeNode(NodeType _type) : type(_type) {
}

TreeNode::TreeNode(NodeType _type, const Token &t) : type(_type), token(t) {}

TreeNode::~TreeNode() {
    while (!child.empty()) {
        delete child.front();
        child.pop_front();
    }
}
