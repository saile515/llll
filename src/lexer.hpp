#pragma once

#include <optional>
#include <string>
#include <vector>

enum TokenType {
    Add,
    Subtract,
    Multiply,
    Divide,
    Power,
    Increment,
    Decrement,
    Assign,
    AddAssign,
    SubtractAssign,
    MultiplyAssign,
    DivideAssign,
    Equals,
    Integer,
    String,
    Let,
    Identifier,
    Semicolon,
    OpenParentheses,
    CloseParentheses
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
};

class Lexer {
private:
    std::string _input;
    unsigned int _offset = 0;

public:
    std::vector<Token> tokens;
    Lexer(std::string input);

private:
    void lex();
    std::optional<char> peek(int offset = 0);
    std::optional<char> consume();
};
