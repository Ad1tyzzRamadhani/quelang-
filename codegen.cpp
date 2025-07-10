// codegen.cpp
#include "ast.hpp"
#include <sstream>
#include <vector>
#include <string>
#include <stack>
#include <unordered_map>
#include <memory>

struct StructLayout {
    std::unordered_map<std::string, int> fieldOffsets;
    int size = 0;
};

class CodegenASM {
    std::vector<std::string> asmLines;
    int labelCount = 0;
    std::stack<std::string> breakLabels;
    std::stack<std::string> continueLabels;
    std::unordered_map<std::string, StructLayout> structLayouts;
    std::unordered_map<std::string, int> localOffsets;
    int currentOffset = 0;

    std::string uniqueLabel(const std::string& base) {
        return base + std::to_string(labelCount++);
    }

    void emit(const std::string& line) {
        asmLines.push_back(line);
    }

    void emitLabel(const std::string& label) {
        asmLines.push_back(label + ":");
    }

public:
    std::string generate(std::shared_ptr<ProgramNode> program) {
        asmLines.clear();
        emit(".text");
        emit(".global _start");
        emit("_start:");
        emit("  bl main");
        emit("  mov x8, #93");
        emit("  mov x0, #0");
        emit("  svc #0");

        for (auto& node : program->topDefs) gen(node);

        std::ostringstream out;
        for (auto& line : asmLines) out << line << "\n";
        return out.str();
    }

    void gen(const NodePtr& node) {
        switch (node->kind) {
            case NodeKind::StructDef:
                genStruct(std::dynamic_pointer_cast<StructDefNode>(node)); break;
            case NodeKind::FunctionDef:
                genFunction(std::dynamic_pointer_cast<FunctionDefNode>(node)); break;
            default: break;
        }
    }

    void genStruct(const std::shared_ptr<StructDefNode>& def) {
        StructLayout layout;
        int offset = 0;
        for (auto& field : def->fields) {
            layout.fieldOffsets[field.first] = offset;
            offset += 8;
        }
        layout.size = offset;
        structLayouts[def->name] = layout;
    }

    void genFunction(const std::shared_ptr<FunctionDefNode>& fn) {
        localOffsets.clear();
        currentOffset = 0;

        emitLabel(fn->name);
        emit("  stp x29, x30, [sp, #-16]!");
        emit("  mov x29, sp");

        int argreg = 0;
        for (auto& param : fn->params) {
            currentOffset += 8;
            localOffsets[param.name] = -currentOffset;
            emit("  str x" + std::to_string(argreg++) + ", [x29, #" + std::to_string(-currentOffset) + "]");
        }

        genBlock(std::dynamic_pointer_cast<BlockNode>(fn->body));

        emit("  ldp x29, x30, [sp], #16");
        emit("  ret");
    }

    void genBlock(const std::shared_ptr<BlockNode>& block) {
        for (auto& stmt : block->statements) genStmt(stmt);
    }

    void genStmt(const NodePtr& stmt) {
        switch (stmt->kind) {
            case NodeKind::Decl: {
                auto d = std::dynamic_pointer_cast<DeclStmtNode>(stmt);
                genExpr(d->expr);
                currentOffset += 8;
                localOffsets[d->name] = -currentOffset;
                emit("  str x0, [x29, #" + std::to_string(-currentOffset) + "]");
                break;
            }
            case NodeKind::Assign: {
                auto a = std::dynamic_pointer_cast<AssignStmtNode>(stmt);
                genExpr(a->expr);
                if (a->lhs->kind == NodeKind::VarRef) {
                    auto v = std::dynamic_pointer_cast<VarRefNode>(a->lhs);
                    if (localOffsets.count(v->name)) {
                        emit("  str x0, [x29, #" + std::to_string(localOffsets[v->name]) + "]");
                    }
                }
                break;
            }
            case NodeKind::PointerAssign: {
                auto p = std::dynamic_pointer_cast<PointerAssignNode>(stmt);
                genExpr(p->valueExpr);
                emit("  mov x1, x0");
                genExpr(p->pointerExpr);
                emit("  str x1, [x0]");
                break;
            }
            case NodeKind::ExprStmt: {
                auto e = std::dynamic_pointer_cast<ExprStmtNode>(stmt);
                genExpr(e->expr); break;
            }
            case NodeKind::Return: {
                auto r = std::dynamic_pointer_cast<ReturnStmtNode>(stmt);
                if (r->expr) genExpr(r->expr);
                emit("  ret"); break;
            }
            case NodeKind::Break: {
                if (!breakLabels.empty()) emit("  b " + breakLabels.top()); break;
            }
            case NodeKind::Continue: {
                if (!continueLabels.empty()) emit("  b " + continueLabels.top()); break;
            }
            case NodeKind::If: {
                auto i = std::dynamic_pointer_cast<IfStmtNode>(stmt);
                std::string endLabel = uniqueLabel("endif");
                for (size_t j = 0; j < i->branches.size(); ++j) {
                    auto& [cond, blk] = i->branches[j];
                    std::string elseLabel = uniqueLabel("else");
                    genExpr(cond);
                    emit("  cbz x0, " + elseLabel);
                    genBlock(std::dynamic_pointer_cast<BlockNode>(blk));
                    emit("  b " + endLabel);
                    emitLabel(elseLabel);
                }
                if (i->elseBlock) genBlock(std::dynamic_pointer_cast<BlockNode>(i->elseBlock));
                emitLabel(endLabel);
                break;
            }
            case NodeKind::While: {
                auto w = std::dynamic_pointer_cast<WhileStmtNode>(stmt);
                std::string begin = uniqueLabel("while_start");
                std::string end = uniqueLabel("while_end");
                breakLabels.push(end);
                continueLabels.push(begin);
                emitLabel(begin);
                genExpr(w->cond);
                emit("  cbz x0, " + end);
                genBlock(std::dynamic_pointer_cast<BlockNode>(w->block));
                emit("  b " + begin);
                emitLabel(end);
                breakLabels.pop();
                continueLabels.pop();
                break;
            }
            case NodeKind::Inj: {
                auto inj = std::dynamic_pointer_cast<InjStmtNode>(stmt);
                for (auto& val : inj->values) {
                    if (auto lit = std::dynamic_pointer_cast<LiteralNode>(val)) {
                        emit(lit->value);
                    }
                }
                break;
            }
            default: break;
        }
    }

    void genExpr(const NodePtr& expr) {
        switch (expr->kind) {
            case NodeKind::Literal: {
                auto lit = std::dynamic_pointer_cast<LiteralNode>(expr);
                emit("  mov x0, #" + lit->value); break;
            }
            case NodeKind::VarRef: {
                auto v = std::dynamic_pointer_cast<VarRefNode>(expr);
                if (localOffsets.count(v->name))
                    emit("  ldr x0, [x29, #" + std::to_string(localOffsets[v->name]) + "]");
                break;
            }
            case NodeKind::Call: {
                auto call = std::dynamic_pointer_cast<CallNode>(expr);
                for (size_t i = 0; i < call->args.size(); ++i) {
                    genExpr(call->args[i]);
                    emit("  mov x" + std::to_string(i) + ", x0");
                }
                emit("  bl " + call->name); break;
            }
            case NodeKind::StructInit: {
                auto s = std::dynamic_pointer_cast<StructInitNode>(expr);
                auto layout = structLayouts[s->name];
                emit("  mov x0, sp");
                emit("  sub sp, sp, #" + std::to_string(layout.size));
                break;
            }
            case NodeKind::UnaryOp: {
                auto un = std::dynamic_pointer_cast<UnaryOpNode>(expr);
                genExpr(un->rhs);
                if (un->op == "*") emit("  ldr x0, [x0]");
                else if (un->op == "-") emit("  neg x0, x0");
                break;
            }
            case NodeKind::BinaryOp: {
                auto bin = std::dynamic_pointer_cast<BinaryOpNode>(expr);
                genExpr(bin->lhs);
                emit("  mov x1, x0");
                genExpr(bin->rhs);
                if (bin->op == "+") emit("  add x0, x1, x0");
                else if (bin->op == "-") emit("  sub x0, x1, x0");
                else if (bin->op == "*") emit("  mul x0, x1, x0");
                else if (bin->op == "/") emit("  udiv x0, x1, x0");
                break;
            }
            default: break;
        }
    }
};
