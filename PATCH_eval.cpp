#ifndef EVAL_CPP
#define EVAL_CPP

#include "ast.hpp"
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>

using Env = std::unordered_map<std::string, NodePtr>;

class Evaluator {
    Env env;
    std::unordered_map<std::string, std::shared_ptr<FunctionDefNode>> funcs;

public:
    void evalProgram(const std::shared_ptr<ProgramNode>& program) {
        for (const auto& def : program->topDefs) {
            if (def->kind == NodeKind::FunctionDef) {
                auto fn = std::dynamic_pointer_cast<FunctionDefNode>(def);
                funcs[fn->name] = fn;
            }
        }

        for (const auto& def : program->topDefs) {
            if (def->kind == NodeKind::TypeInit) {
                auto tinit = std::dynamic_pointer_cast<TypeInitNode>(def);
                env[tinit->name] = std::make_shared<LiteralNode>(tinit->type, def->line);
            } else if (def->kind == NodeKind::Block) {
                eval(def);
            }
        }
    }

    NodePtr eval(NodePtr node) {
        switch (node->kind) {
            case NodeKind::Literal:
            case NodeKind::StructInit:
                return node;

            case NodeKind::VarRef: {
                auto var = std::dynamic_pointer_cast<VarRefNode>(node);
                if (env.count(var->name)) return env[var->name];
                throw std::runtime_error("Undefined variable: " + var->name);
            }

            case NodeKind::UnaryOp: {
                auto un = std::dynamic_pointer_cast<UnaryOpNode>(node);
                auto rhs = eval(un->rhs);
                if (auto lit = std::dynamic_pointer_cast<LiteralNode>(rhs)) {
                    if (un->op == "-") return std::make_shared<LiteralNode>(std::to_string(-std::stoi(lit->value)), lit->line);
                }
                throw std::runtime_error("Unsupported unary op or non-literal");
            }

            case NodeKind::BinaryOp: {
                auto bin = std::dynamic_pointer_cast<BinaryOpNode>(node);
                auto lhs = eval(bin->lhs);
                auto rhs = eval(bin->rhs);
                return evalBinary(bin->op, lhs, rhs, bin->line);
            }

            case NodeKind::ArrayLiteral: {
                auto arr = std::dynamic_pointer_cast<ArrayLiteralNode>(node);
                auto newArr = std::make_shared<ArrayLiteralNode>(arr->line);
                for (auto& el : arr->elements) newArr->elements.push_back(eval(el));
                return newArr;
            }

            case NodeKind::MemberAccess: {
                auto mem = std::dynamic_pointer_cast<MemberAccessNode>(node);
                auto base = eval(mem->object);
                if (auto lit = std::dynamic_pointer_cast<LiteralNode>(base)) {
                    return std::make_shared<LiteralNode>(lit->value + "." + mem->field, mem->line);
                }
                return std::make_shared<MemberAccessNode>(base, mem->field);
            }

            case NodeKind::Call: {
                auto call = std::dynamic_pointer_cast<CallNode>(node);
                std::vector<NodePtr> args;
                for (auto& arg : call->args) args.push_back(eval(arg));
                return evalCall(call->name, args, call->line);
            }

            case NodeKind::Decl: {
                auto decl = std::dynamic_pointer_cast<DeclStmtNode>(node);
                env[decl->name] = eval(decl->expr);
                return env[decl->name];
            }

            case NodeKind::Block: {
                auto blk = std::dynamic_pointer_cast<BlockNode>(node);
                for (auto& stmt : blk->statements) {
                    NodePtr res = eval(stmt);
                    if (res && res->kind == NodeKind::Return) return res;
                }
                return nullptr;
            }

            case NodeKind::If: {
                auto ifn = std::dynamic_pointer_cast<IfStmtNode>(node);
                for (auto& [cond, block] : ifn->branches) {
                    auto val = eval(cond);
                    if (asLiteral(val)->value != "0") return eval(block);
                }
                if (ifn->elseBlock) return eval(ifn->elseBlock);
                return nullptr;
            }

            case NodeKind::While: {
                auto wn = std::dynamic_pointer_cast<WhileStmtNode>(node);
                while (true) {
                    auto cond = eval(wn->cond);
                    if (asLiteral(cond)->value == "0") break;
                    eval(wn->block);
                }
                return nullptr;
            }

            case NodeKind::Return: {
                auto ret = std::dynamic_pointer_cast<ReturnStmtNode>(node);
                if (ret->expr) return eval(ret->expr);
                return std::make_shared<LiteralNode>("0", node->line);
            }

            default:
                return node;
        }
    }

private:
    NodePtr evalBinary(const std::string& op, NodePtr l, NodePtr r, int line) {
        auto lhs = std::dynamic_pointer_cast<LiteralNode>(l);
        auto rhs = std::dynamic_pointer_cast<LiteralNode>(r);
        if (!lhs || !rhs) throw std::runtime_error("Operands must be literals");

        if (op == "+") {
            bool isNum = isNumber(lhs->value) && isNumber(rhs->value);
            return std::make_shared<LiteralNode>(
                isNum ? std::to_string(std::stoi(lhs->value) + std::stoi(rhs->value)) : lhs->value + rhs->value,
                line
            );
        }
        if (op == "-") return makeNum(std::stoi(lhs->value) - std::stoi(rhs->value), line);
        if (op == "*") return makeNum(std::stoi(lhs->value) * std::stoi(rhs->value), line);
        if (op == "/") return makeNum(std::stoi(lhs->value) / std::stoi(rhs->value), line);
        if (op == "%") return makeNum(std::stoi(lhs->value) % std::stoi(rhs->value), line);

        throw std::runtime_error("Unsupported binary operator: " + op);
    }

    NodePtr evalCall(const std::string& name, const std::vector<NodePtr>& args, int line) {
        if (name == "abs") return makeNum(std::abs(std::stoi(asLiteral(args[0])->value)), line);
        if (name == "sqrt") return makeNum((int)std::sqrt(std::stoi(asLiteral(args[0])->value)), line);
        if (name == "max") return makeNum(std::max(std::stoi(asLiteral(args[0])->value), std::stoi(asLiteral(args[1])->value)), line);
        if (name == "min") return makeNum(std::min(std::stoi(asLiteral(args[0])->value), std::stoi(asLiteral(args[1])->value)), line);
        if (name == "pow") return makeNum((int)std::pow(std::stoi(asLiteral(args[0])->value), std::stoi(asLiteral(args[1])->value)), line);

        if (funcs.count(name)) {
            auto fn = funcs[name];
            Env saved = env;
            for (size_t i = 0; i < fn->params.size(); ++i) {
                env[fn->params[i].name] = args[i];
            }
            auto res = eval(fn->body);
            env = saved;
            return res;
        }

        throw std::runtime_error("Unknown function: " + name);
    }

    std::shared_ptr<LiteralNode> asLiteral(NodePtr node) {
        auto lit = std::dynamic_pointer_cast<LiteralNode>(node);
        if (!lit) throw std::runtime_error("Expected literal");
        return lit;
    }

    NodePtr makeNum(int v, int line) {
        return std::make_shared<LiteralNode>(std::to_string(v), line);
    }

    bool isNumber(const std::string& val) {
        return !val.empty() && (isdigit(val[0]) || (val[0] == '-' && val.size() > 1 && isdigit(val[1])));
    }
};

#endif

        

            case NodeKind::UnaryOp: {
                auto un = std::dynamic_pointer_cast<UnaryOpNode>(node);
                auto rhs = eval(un->rhs);
                if (auto lit = std::dynamic_pointer_cast<LiteralNode>(rhs)) {
                    if (un->op == "-") return std::make_shared<LiteralNode>(std::to_string(-std::stoi(lit->value)), lit->line);
                }
                throw std::runtime_error("Unsupported unary op or non-literal");
            }

            case NodeKind::BinaryOp: {
                auto bin = std::dynamic_pointer_cast<BinaryOpNode>(node);
                auto lhs = eval(bin->lhs);
                auto rhs = eval(bin->rhs);
                return evalBinary(bin->op, lhs, rhs, bin->line);
            }

            case NodeKind::ArrayLiteral: {
                auto arr = std::dynamic_pointer_cast<ArrayLiteralNode>(node);
                auto newArr = std::make_shared<ArrayLiteralNode>(arr->line);
                for (auto& el : arr->elements) newArr->elements.push_back(eval(el));
                return newArr;
            }

            case NodeKind::MemberAccess: {
                auto mem = std::dynamic_pointer_cast<MemberAccessNode>(node);
                auto base = eval(mem->object);
                if (auto lit = std::dynamic_pointer_cast<LiteralNode>(base)) {
                    return std::make_shared<LiteralNode>(lit->value + "." + mem->field, mem->line);
                }
                return std::make_shared<MemberAccessNode>(base, mem->field);
            }

            case NodeKind::Call: {
                auto call = std::dynamic_pointer_cast<CallNode>(node);
                std::vector<NodePtr> args;
                for (auto& arg : call->args) args.push_back(eval(arg));
                return evalCall(call->name, args, call->line);
            }

            case NodeKind::Decl: {
                auto decl = std::dynamic_pointer_cast<DeclStmtNode>(node);
                env[decl->name] = eval(decl->expr);
                return env[decl->name];
            }

            case NodeKind::Block: {
                auto blk = std::dynamic_pointer_cast<BlockNode>(node);
                for (auto& stmt : blk->statements) {
                    NodePtr res = eval(stmt);
                    if (res && res->kind == NodeKind::Return) return res;
                }
                return nullptr;
            }

            case NodeKind::If: {
                auto ifn = std::dynamic_pointer_cast<IfStmtNode>(node);
                for (auto& [cond, block] : ifn->branches) {
                    auto val = eval(cond);
                    if (asLiteral(val)->value != "0") return eval(block);
                }
                if (ifn->elseBlock) return eval(ifn->elseBlock);
                return nullptr;
            }

            case NodeKind::While: {
                auto wn = std::dynamic_pointer_cast<WhileStmtNode>(node);
                while (true) {
                    auto cond = eval(wn->cond);
                    if (asLiteral(cond)->value == "0") break;
                    eval(wn->block);
                }
                return nullptr;
            }

            case NodeKind::Return: {
                auto ret = std::dynamic_pointer_cast<ReturnStmtNode>(node);
                if (ret->expr) return eval(ret->expr);
                return std::make_shared<LiteralNode>("0", node->line);
            }

            default:
                return node;
        }
    }

private:
    NodePtr evalBinary(const std::string& op, NodePtr l, NodePtr r, int line) {
        auto lhs = std::dynamic_pointer_cast<LiteralNode>(l);
        auto rhs = std::dynamic_pointer_cast<LiteralNode>(r);
        if (!lhs || !rhs) throw std::runtime_error("Operands must be literals");

        if (op == "+") {
            bool isNum = isNumber(lhs->value) && isNumber(rhs->value);
            return std::make_shared<LiteralNode>(
                isNum ? std::to_string(std::stoi(lhs->value) + std::stoi(rhs->value)) : lhs->value + rhs->value,
                line
            );
        }
        if (op == "-") return makeNum(std::stoi(lhs->value) - std::stoi(rhs->value), line);
        if (op == "*") return makeNum(std::stoi(lhs->value) * std::stoi(rhs->value), line);
        if (op == "/") return makeNum(std::stoi(lhs->value) / std::stoi(rhs->value), line);
        if (op == "%") return makeNum(std::stoi(lhs->value) % std::stoi(rhs->value), line);

        throw std::runtime_error("Unsupported binary operator: " + op);
    }

    NodePtr evalCall(const std::string& name, const std::vector<NodePtr>& args, int line) {
        if (name == "abs") return makeNum(std::abs(std::stoi(asLiteral(args[0])->value)), line);
        if (name == "sqrt") return makeNum((int)std::sqrt(std::stoi(asLiteral(args[0])->value)), line);
        if (name == "max") return makeNum(std::max(std::stoi(asLiteral(args[0])->value), std::stoi(asLiteral(args[1])->value)), line);
        if (name == "min") return makeNum(std::min(std::stoi(asLiteral(args[0])->value), std::stoi(asLiteral(args[1])->value)), line);
        if (name == "pow") return makeNum((int)std::pow(std::stoi(asLiteral(args[0])->value), std::stoi(asLiteral(args[1])->value)), line);

        if (funcs.count(name)) {
            auto fn = funcs[name];
            Env saved = env;
            for (size_t i = 0; i < fn->params.size(); ++i) {
                env[fn->params[i].name] = args[i];
            }
            auto res = eval(fn->body);
            env = saved;
            return res;
        }

        throw std::runtime_error("Unknown function: " + name);
    }

    std::shared_ptr<LiteralNode> asLiteral(NodePtr node) {
        auto lit = std::dynamic_pointer_cast<LiteralNode>(node);
        if (!lit) throw std::runtime_error("Expected literal");
        return lit;
    }

    NodePtr makeNum(int v, int line) {
        return std::make_shared<LiteralNode>(std::to_string(v), line);
    }

    bool isNumber(const std::string& val) {
        return !val.empty() && (isdigit(val[0]) || (val[0] == '-' && val.size() > 1 && isdigit(val[1])));
    }
};

#endif
