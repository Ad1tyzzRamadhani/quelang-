#include "ast.hpp"
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <memory>

class SemanticChecker {
    struct VarInfo {
        std::string type;
        bool isDefined = false;
    };

    std::unordered_map<std::string, VarInfo> vars;
    std::unordered_map<std::string, std::string> functions;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> structFields;

public:
    void check(std::shared_ptr<ProgramNode> program) {
        for (auto& def : program->topDefs) {
            if (def->kind == NodeKind::FunctionDef) {
                auto fn = std::dynamic_pointer_cast<FunctionDefNode>(def);
                functions[fn->name] = fn->returnType;
            } else if (def->kind == NodeKind::StructDef) {
                auto s = std::dynamic_pointer_cast<StructDefNode>(def);
                for (auto& field : s->fields) {
                    structFields[s->name][field.first] = field.second;
                }
            }
        }

        for (auto& def : program->topDefs) {
            if (def->kind == NodeKind::FunctionDef) {
                checkFunction(std::dynamic_pointer_cast<FunctionDefNode>(def));
            }
        }
    }

private:
    void checkFunction(std::shared_ptr<FunctionDefNode> fn) {
        vars.clear();
        for (auto& param : fn->params) {
            vars[param.name] = {param.type, true};
        }
        checkBlock(std::dynamic_pointer_cast<BlockNode>(fn->body), fn->returnType);
    }

    void checkBlock(const std::shared_ptr<BlockNode>& block, const std::string& expectedReturnType) {
        for (auto& stmt : block->statements) {
            checkStmt(stmt, expectedReturnType);
        }
    }

    void checkStmt(const NodePtr& stmt, const std::string& expectedReturnType) {
        switch (stmt->kind) {
            case NodeKind::Decl: {
                auto d = std::dynamic_pointer_cast<DeclStmtNode>(stmt);
                std::string actualType = checkExpr(d->expr);
                if (!d->type.empty() && d->type != actualType) {
                    throw std::runtime_error("Type mismatch in declaration of " + d->name);
                }
                vars[d->name] = {d->type.empty() ? actualType : d->type, true};
                break;
            }
            case NodeKind::Assign: {
                auto a = std::dynamic_pointer_cast<AssignStmtNode>(stmt);
                std::string rhsType = checkExpr(a->expr);
                if (a->lhs->kind == NodeKind::VarRef) {
                    auto v = std::dynamic_pointer_cast<VarRefNode>(a->lhs);
                    if (!vars.count(v->name)) {
                        throw std::runtime_error("Undefined variable: " + v->name);
                    }
                    if (vars[v->name].type != rhsType) {
                        throw std::runtime_error("Type mismatch in assignment to " + v->name);
                    }
                }
                break;
            }
            case NodeKind::ExprStmt: {
                checkExpr(std::dynamic_pointer_cast<ExprStmtNode>(stmt)->expr);
                break;
            }
            case NodeKind::Return: {
                auto r = std::dynamic_pointer_cast<ReturnStmtNode>(stmt);
                if (r->expr) {
                    std::string retType = checkExpr(r->expr);
                    if (expectedReturnType != retType) {
                        throw std::runtime_error("Return type mismatch");
                    }
                }
                break;
            }
            case NodeKind::If: {
                auto i = std::dynamic_pointer_cast<IfStmtNode>(stmt);
                for (auto& [cond, blk] : i->branches) {
                    std::string condType = checkExpr(cond);
                    if (condType != "bool") {
                        throw std::runtime_error("If condition must be bool");
                    }
                    checkBlock(std::dynamic_pointer_cast<BlockNode>(blk), expectedReturnType);
                }
                if (i->elseBlock) {
                    checkBlock(std::dynamic_pointer_cast<BlockNode>(i->elseBlock), expectedReturnType);
                }
                break;
            }
            case NodeKind::While: {
                auto w = std::dynamic_pointer_cast<WhileStmtNode>(stmt);
                std::string condType = checkExpr(w->cond);
                if (condType != "bool") {
                    throw std::runtime_error("While condition must be bool");
                }
                checkBlock(std::dynamic_pointer_cast<BlockNode>(w->block), expectedReturnType);
                break;
            }
            default: break;
        }
    }

    std::string checkExpr(const NodePtr& expr) {
        switch (expr->kind) {
            case NodeKind::Literal: {
                auto lit = std::dynamic_pointer_cast<LiteralNode>(expr);
                if (lit->value == "true" || lit->value == "false") return "bool";
                if (isdigit(lit->value[0])) return "u16";
                return "str";
            }
            case NodeKind::VarRef: {
                auto v = std::dynamic_pointer_cast<VarRefNode>(expr);
                if (!vars.count(v->name)) throw std::runtime_error("Undefined variable: " + v->name);
                return vars[v->name].type;
            }
            case NodeKind::Call: {
                auto c = std::dynamic_pointer_cast<CallNode>(expr);
                if (!functions.count(c->name)) throw std::runtime_error("Undefined function: " + c->name);
                return functions[c->name];
            }
            case NodeKind::BinaryOp: {
                auto b = std::dynamic_pointer_cast<BinaryOpNode>(expr);
                std::string lhs = checkExpr(b->lhs);
                std::string rhs = checkExpr(b->rhs);
                if (lhs != rhs) throw std::runtime_error("Binary op type mismatch");
                if (b->op == "==" || b->op == "!=" || b->op == "<" || b->op == ">") return "bool";
                return lhs;
            }
            case NodeKind::UnaryOp: {
                auto u = std::dynamic_pointer_cast<UnaryOpNode>(expr);
                return checkExpr(u->rhs);
            }
            default: return "u16";
        }
    }
};
