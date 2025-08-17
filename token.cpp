#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_set>

enum TokenType {
    IDENT, NUMBER, STRING, KEYWORD, SYMBOL, NEWLINE, END
};

struct Token {
    TokenType type;
    std::string value;
    int line;

    Token(TokenType t, const std::string &v, int l) : type(t), value(v), line(l) {}
};

const std::unordered_set<std::string> keywords = {
    "init", "def", "type", "struct", "var", "if", "else", "elseif", "while",
    "return", "break", "continue", "true", "false", "nil", "inj", "u16", "bool", "str", "and", "or", "int", "float", "i8", "char", "u8", "i16", "byte", "i32", "u32", "u64", "i64",
    "any", "void", "opaque", "error"
};

class Tokenizer {
    std::string src;
    size_t pos = 0;
    int line = 1;
    bool debug;

public:
    Tokenizer(const std::string &input, bool dbg = false) : src(input), debug(dbg) {}

    char peek() {
        return pos < src.size() ? src[pos] : '\0';
    }

    char get() {
        char c = peek();
        if (c == '\n') line++;
        pos++;
        return c;
    }

    void skipWhitespace() {
        while (isspace(peek()) && peek() != '\n') get();
    }

    Token nextToken() {
        skipWhitespace();
        char c = peek();
        if (c == '\0') return Token(END, "", line);
        if (c == '\n') {
            get();
            return Token(NEWLINE, "\\n", line - 1);
        }

        if (isalpha(c) || c == '_') {
            std::string ident;
            while (isalnum(peek()) || peek() == '_') ident += get();
            if (keywords.count(ident)) return Token(KEYWORD, ident, line);
            return Token(IDENT, ident, line);
        }

        if (isdigit(c)) {
            std::string number;
            while (isdigit(peek())) number += get();
            return Token(NUMBER, number, line);
        }

        if (c == '"') {
            std::string str;
            get(); // skip "
            while (peek() && peek() != '"') {
                if (peek() == '\\') {
                    get();
                    char esc = get();
                    switch (esc) {
                        case 'n': str += '\n'; break;
                        case 'r': str += '\r'; break;
                        case 't': str += '\t'; break;
                        case '"': str += '"'; break;
                        case '\\': str += '\\'; break;
                        default: str += esc;
                    }
                } else {
                    str += get();
                }
            }
            get(); // closing "
            return Token(STRING, str, line);
        }

        // Symbols & operators
        std::string sym;
        sym += get();

        char next = peek();
        if ((sym == "=" || sym == "!" || sym == "<" || sym == ">") && next == '=') {
            sym += get(); // Dapatkan '=' untuk kombinasi ==, !=, <=, >=
        }

        return Token(SYMBOL, sym, line);
    }

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        Token tok = nextToken();
        while (tok.type != END) {
            if (debug) std::cout << "[" << tok.line << "] " << tok.value << " (" << tokTypeName(tok.type) << ")\n";
            tokens.push_back(tok);
            tok = nextToken();
        }
        return tokens;
    }

    std::string tokTypeName(TokenType type) {
        switch (type) {
            case IDENT: return "IDENT";
            case NUMBER: return "NUMBER";
            case STRING: return "STRING";
            case KEYWORD: return "KEYWORD";
            case SYMBOL: return "SYMBOL";
            case NEWLINE: return "NEWLINE";
            default: return "END";
        }
    }
};
