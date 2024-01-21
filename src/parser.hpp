#pragma once

#include "lexer.hpp"
#include <optional>
#include <string>
#include <vector>

struct SyntaxNode {};

struct SyntaxNodeExpression : SyntaxNode {};

struct SyntaxNodeExpressionInteger : SyntaxNodeExpression {
    std::string value;
};

struct SyntaxNodeExpressionCompare : SyntaxNodeExpression {
    SyntaxNodeExpression left;
    SyntaxNodeExpression right;
    TokenType operator_;
};

struct SyntaxNodeExpressionArithmetic : SyntaxNodeExpression {
    SyntaxNodeExpression left;
    SyntaxNodeExpression right;
    TokenType operator_;
};

struct SyntaxNodeBody : SyntaxNode {
    std::vector<SyntaxNode> content;
};

struct SyntaxNodeLet : SyntaxNode {
    std::string identifier;
    SyntaxNodeExpression value;
};

class Parser {
private:
    std::vector<Token> _tokens;
    int _offset = 0;
    SyntaxNodeBody _global_body;

public:
    Parser(std::vector<Token> tokens);

private:
    void parse();
    std::optional<Token> peek(int offset = 0);
    std::optional<Token> consume();
};
