#include "ast.hpp"
#include "token.cpp"
#include <stdexcept>
#include <memory>

class Parser {
    std::vector<Token> tokens;
    size_t index = 0;
public:
    Parser(const std::vector<Token>& toks) : tokens(toks) {}

    Token peek() {
        return index < tokens.size() ? tokens[index] : Token(END, "", -1);
    }

    Token get() {
        return index < tokens.size() ? tokens[index++] : Token(END, "", -1);
    }

    bool accept(TokenType type, const std::string& val = "") {
        Token t = peek();
        if (t.type == type && (val.empty() || t.value == val)) {
            index++;
            return true;
        }
        return false;
    }

    void expect(TokenType type, const std::string& val = "") {
        Token t = get();
        if (t.type != type || (!val.empty() && t.value != val)) {
            throw std::runtime_error("Unexpected token at line " + std::to_string(t.line) + ": expected " + val);
        }
    }

    void skipNewlines() {
        while (accept(NEWLINE));
    }

    std::string expectIdent() {
        Token t = get();
        if (t.type != IDENT) {
            throw std::runtime_error("Expected identifier at line " + std::to_string(t.line));
        }
        return t.value;
    }

    std::string expectType() {
        Token t = get();
        if (t.type == KEYWORD || t.type == IDENT) {
            std::string base = t.value;
            if (accept(SYMBOL, "*")) {
                return "*" + expectType();
            }
            return base;
        }
        throw std::runtime_error("Expected type at line " + std::to_string(t.line));
    }

    std::shared_ptr<ProgramNode> parseProgram() {
        auto program = std::make_shared<ProgramNode>();
        skipNewlines();
        while (peek().type != END) {
            program->topDefs.push_back(parseTopDef());
            skipNewlines();
        }
        return program;
    }

    NodePtr parseTopDef() {
        skipNewlines();
        Token t = peek();

        if (accept(KEYWORD, "init")) {
            if (accept(KEYWORD, "type")) {
                std::string name = expectIdent();
                expect(SYMBOL, "=");
                std::string typ = expectType();
                return std::make_shared<TypeInitNode>(name, typ, t.line);
            } else {
                return parseBlock();
            }
        }

        if (accept(KEYWORD, "def")) {
            if (accept(KEYWORD, "struct")) {
                std::string name = expectIdent();
                expect(SYMBOL, "{");
                auto def = std::make_shared<StructDefNode>(name, t.line);
                while (!accept(SYMBOL, "}")) {
                    std::string field = expectIdent();
                    expect(SYMBOL, ":");
                    std::string type = expectType();
                    def->fields.emplace_back(field, type);
                    skipNewlines();
                }
                return def;
            } else {
                std::string name = expectIdent();
                expect(SYMBOL, "(");
                std::vector<ParamNode> params;
                if (!accept(SYMBOL, ")")) {
                    do {
                        std::string pname = expectIdent();
                        expect(SYMBOL, ":");
                        std::string ptype = expectType();
                        params.emplace_back(pname, ptype, t.line);
                    } while (accept(SYMBOL, ","));
                    expect(SYMBOL, ")");
                }
                std::string retType = "";
                if (accept(SYMBOL, ":")) {
                    retType = expectType();
                }
                auto fn = std::make_shared<FunctionDefNode>();
                fn->name = name;
                fn->params = params;
                fn->returnType = retType;
                fn->body = parseBlock();
                return fn;
            }
        }

        throw std::runtime_error("Invalid top-level definition at line " + std::to_string(t.line));
    }

    std::shared_ptr<BlockNode> parseBlock() {
        expect(SYMBOL, "{");
        auto block = std::make_shared<BlockNode>();
        skipNewlines();
        while (!accept(SYMBOL, "}")) {
            block->statements.push_back(parseStmt());
            skipNewlines();
        }
        return block;
    }

    NodePtr parseStmt() {
        Token t = peek();
        if (accept(KEYWORD, "let")) {
            std::string name = expectIdent();
            std::string type = "";
            if (accept(SYMBOL, ":")) {
                type = expectType();
            }
            expect(SYMBOL, "=");
            NodePtr expr = parseExpr();
            return std::make_shared<DeclStmtNode>(DeclStmtNode{name, type, expr});
        } else if (accept(KEYWORD, "if")) {
            auto cond = parseExpr();
            auto block = parseBlock();
            auto node = std::make_shared<IfStmtNode>();
            node->branches.emplace_back(cond, block);
            while (accept(KEYWORD, "elseif")) {
                auto cond2 = parseExpr();
                auto block2 = parseBlock();
                node->branches.emplace_back(cond2, block2);
            }
            if (accept(KEYWORD, "else")) {
                node->elseBlock = parseBlock();
            }
            return node;
        } else if (accept(KEYWORD, "while")) {
            auto cond = parseExpr();
            auto block = parseBlock();
            auto node = std::make_shared<WhileStmtNode>();
            node->cond = cond;
            node->block = block;
            return node;
        } else if (accept(KEYWORD, "return")) {
            NodePtr expr = nullptr;
            if (peek().type != NEWLINE && peek().type != SYMBOL) {
                expr = parseExpr();
            }
            auto node = std::make_shared<ReturnStmtNode>();
            node->expr = expr;
            return node;
        } else if (accept(KEYWORD, "break")) {
            return std::make_shared<BreakStmtNode>();
        } else if (accept(KEYWORD, "continue")) {
            return std::make_shared<ContinueStmtNode>();
        } else if (accept(KEYWORD, "inj")) {
            std::string target = expectIdent();
            expect(SYMBOL, "{");
            auto inj = std::make_shared<InjStmtNode>(target);
            while (!accept(SYMBOL, "}")) {
                inj->values.push_back(parseExpr());
                skipNewlines();
            }
            return inj;
        } else {
            NodePtr lhs = parseAssignableExpr();
            if (accept(SYMBOL, "=")) {
                NodePtr rhs = parseExpr();
                if (lhs->kind == NodeKind::UnaryOp) {
                    auto un = std::dynamic_pointer_cast<UnaryOpNode>(lhs);
                    if (un->op == "*") {
                        return std::make_shared<PointerAssignNode>(un->rhs, rhs);
                    }
                }
                auto stmt = std::make_shared<AssignStmtNode>();
                stmt->lhs = lhs;
                stmt->expr = rhs;
                return stmt;
            } else {
                auto stmt = std::make_shared<ExprStmtNode>();
                stmt->expr = lhs;
                return stmt;
            }
        }
    }

    NodePtr parseExpr() {
        NodePtr left = parseAssignableExpr();
        while (isOperator(peek())) {
            std::string op = get().value;
            NodePtr right = parseAssignableExpr();
            left = std::make_shared<BinaryOpNode>(op, left, right);
        }
        return left;
    }

    NodePtr parseAssignableExpr() {
        return parseSimpleExpr();
    }

    NodePtr parseSimpleExpr() {
        Token t = get();
        if (t.type == NUMBER || t.type == STRING || t.value == "true" || t.value == "false" || t.value == "nil") {
            return std::make_shared<LiteralNode>(t.value, t.line);
        } else if (t.type == IDENT) {
            if (accept(SYMBOL, "(")) {
                std::vector<NodePtr> args;
                if (!accept(SYMBOL, ")")) {
                    do {
                        args.push_back(parseExpr());
                    } while (accept(SYMBOL, ","));
                    expect(SYMBOL, ")");
                }
                auto call = std::make_shared<CallNode>(t.value, t.line);
                call->args = args;
                return call;
            } else if (accept(SYMBOL, "(") && accept(SYMBOL, ")")) {
                return std::make_shared<StructInitNode>(t.value, t.line);
            } else {
                return std::make_shared<VarRefNode>(t.value, t.line);
            }
        } else if (t.value == "(") {
            NodePtr expr = parseExpr();
            expect(SYMBOL, ")");
            return expr;
        } else if (t.value == "-" || t.value == "*" || t.value == "&") {
            NodePtr rhs = parseSimpleExpr();
            return std::make_shared<UnaryOpNode>(t.value, rhs);
        }

        throw std::runtime_error("Unexpected expression token at line " + std::to_string(t.line));
    }

    bool isOperator(const Token& t) {
        static std::unordered_set<std::string> ops = {
            "+", "-", "*", "/", "%", "==", "!=", "<", ">", "<=", ">="
        };
        return t.type == SYMBOL && ops.count(t.value);
    }
};
