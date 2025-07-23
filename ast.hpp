#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>

enum class NodeKind {
    Program, FunctionDef, InitBlock, TypeInit,
    StructDef, Param, Block, Decl, Assign, ExprStmt,
    If, While, Return, Break, Continue, Inj,
    Literal, BinaryOp, UnaryOp, VarRef, Call, StructInit,
    PointerAssign, MemberAccess
};

struct Node {
    NodeKind kind;
    int line;
    virtual ~Node() {}
};

using NodePtr = std::shared_ptr<Node>;

template<typename T>
using NodeList = std::vector<std::shared_ptr<T>>;

struct ProgramNode : Node {
    NodeList<Node> topDefs;
    ProgramNode() { kind = NodeKind::Program; }
};

struct ParamNode : Node {
    std::string name;
    std::string type;
    ParamNode(const std::string& n, const std::string& t, int l) : name(n), type(t) { kind = NodeKind::Param; line = l; }
};

struct BlockNode : Node {
    NodeList<Node> statements;
    BlockNode() { kind = NodeKind::Block; }
};

struct FunctionDefNode : Node {
    std::string name;
    std::vector<ParamNode> params;
    std::string returnType;
    NodePtr body;
    FunctionDefNode() { kind = NodeKind::FunctionDef; }
};

struct TypeInitNode : Node {
    std::string name;
    std::string type;
    TypeInitNode(const std::string& n, const std::string& t, int l) : name(n), type(t) {
        kind = NodeKind::TypeInit;
        line = l;
    }
};

struct StructDefNode : Node {
    std::string name;
    std::vector<std::pair<std::string, std::string>> fields;
    StructDefNode(const std::string& n, int l) : name(n) {
        kind = NodeKind::StructDef;
        line = l;
    }
};

struct DeclStmtNode : Node {
    std::string name;
    std::string type;
    NodePtr expr;
    DeclStmtNode(const std::string& n, const std::string& t, NodePtr e)
        : name(n), type(t), expr(e) {
        kind = NodeKind::Decl;
    }
};

struct AssignStmtNode : Node {
    NodePtr lhs;
    NodePtr expr;
    AssignStmtNode() { kind = NodeKind::Assign; }
};

struct ExprStmtNode : Node {
    NodePtr expr;
    ExprStmtNode() { kind = NodeKind::ExprStmt; }
};

struct IfStmtNode : Node {
    std::vector<std::pair<NodePtr, NodePtr>> branches;
    NodePtr elseBlock;
    IfStmtNode() { kind = NodeKind::If; }
};

struct WhileStmtNode : Node {
    NodePtr cond;
    NodePtr block;
    WhileStmtNode() { kind = NodeKind::While; }
};

struct ReturnStmtNode : Node {
    NodePtr expr;
    ReturnStmtNode() { kind = NodeKind::Return; }
};

struct InjStmtNode : Node {
    std::string target;
    std::vector<NodePtr> values;
    InjStmtNode(const std::string& tgt) : target(tgt) {
        kind = NodeKind::Inj;
    }
};

struct BreakStmtNode : Node {
    BreakStmtNode() { kind = NodeKind::Break; }
};

struct ContinueStmtNode : Node {
    ContinueStmtNode() { kind = NodeKind::Continue; }
};

struct LiteralNode : Node {
    std::string value;
    LiteralNode(const std::string& v, int l) : value(v) {
        kind = NodeKind::Literal;
        line = l;
    }
};

struct VarRefNode : Node {
    std::string name;
    VarRefNode(const std::string& v, int l) : name(v) {
        kind = NodeKind::VarRef;
        line = l;
    }
};

struct CallNode : Node {
    std::string name;
    std::vector<NodePtr> args;
    CallNode(const std::string& n, int l) : name(n) {
        kind = NodeKind::Call;
        line = l;
    }
};

struct StructInitNode : Node {
    std::string name;
    StructInitNode(const std::string& n, int l) : name(n) {
        kind = NodeKind::StructInit;
        line = l;
    }
};

struct UnaryOpNode : Node {
    std::string op;
    NodePtr rhs;
    UnaryOpNode(const std::string& o, NodePtr r) : op(o), rhs(r) {
        kind = NodeKind::UnaryOp;
        line = r->line;
    }
};

struct BinaryOpNode : Node {
    std::string op;
    NodePtr lhs, rhs;
    BinaryOpNode(const std::string& o, NodePtr l, NodePtr r) : op(o), lhs(l), rhs(r) {
        kind = NodeKind::BinaryOp;
        line = l->line;
    }
};

struct PointerAssignNode : Node {
    NodePtr pointerExpr;
    NodePtr valueExpr;
    PointerAssignNode(NodePtr ptr, NodePtr val) : pointerExpr(ptr), valueExpr(val) {
        kind = NodeKind::PointerAssign;
        line = ptr->line;
    }
};

struct MemberAccessNode : Node {
    NodePtr object;
    std::string field;
    MemberAccessNode(NodePtr obj, const std::string& fld) : object(obj), field(fld) {
        kind = NodeKind::MemberAccess;
        line = obj->line;
    }
};

#endif
