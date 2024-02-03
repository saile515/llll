#pragma once

#include "lexer.hpp"
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

struct SyntaxNodeExpression;
struct SyntaxNodeStatement;

struct SyntaxNodeExpressionInteger {
    std::string value;
};

struct SyntaxNodeExpressionCompare {
    std::unique_ptr<SyntaxNodeExpression> left;
    std::unique_ptr<SyntaxNodeExpression> right;
    TokenType operator_;
};

struct SyntaxNodeExpressionArithmetic {
    std::unique_ptr<SyntaxNodeExpression> left;
    std::unique_ptr<SyntaxNodeExpression> right;
    TokenType operator_;
};

struct SyntaxNodeBody {
    std::vector<SyntaxNodeStatement> content;
};

struct SyntaxNodeStatementLet {
    std::string identifier;
    std::unique_ptr<SyntaxNodeExpression> value;
};

struct SyntaxNodeExpression
  : std::variant<SyntaxNodeExpressionInteger,
                 SyntaxNodeExpressionCompare,
                 SyntaxNodeExpressionArithmetic> {};

struct SyntaxNodeStatement : std::variant<SyntaxNodeStatementLet> {};

class Parser {
private:
    std::vector<Token> _tokens;
    int _offset = 0;

public:
    SyntaxNodeBody global_body;

    Parser(std::vector<Token> tokens)
      : _tokens(tokens) {
        parse();
    }

private:
    std::unique_ptr<SyntaxNodeExpression> expression_from_tokens(
        std::vector<Token> tokens) {
        int max_precedence = 0;

        while (true) {
            for (int i = 0; i < tokens.size(); i++) {
                int precedence;
                switch (tokens[i].type) {
                    case TokenType::Add:
                    case TokenType::Subtract:
                        precedence = 0;
                        break;
                    case TokenType::Multiply:
                    case TokenType::Divide:
                        precedence = 1;
                        break;
                    case TokenType::Power:
                        precedence = 2;
                        break;
                    case TokenType::OpenParentheses:
                        precedence = 3;
                        break;
                    default:
                        precedence = 4;
                        break;
                }

                if (tokens[i].type == TokenType::OpenParentheses) {
                    int offset = 1;
                    int open = 1; // Start at 0 since check happens before
                                  // increment of last closing parentheses.
                    int close = 0;

                    while (true) {
                        if (tokens.size() < i + offset) {
                            exit(1);
                        }

                        if (tokens[i + offset].type ==
                            TokenType::OpenParentheses) {
                            open++;
                        } else if (tokens[i + offset].type ==
                                   TokenType::CloseParentheses) {
                            close++;
                            if (open == close) {
                                break;
                            }
                        }
                        offset++;
                    }

                    if (max_precedence == precedence) {
                        std::vector<Token> enclosed_tokens(
                            tokens.begin() + i + 1,
                            tokens.begin() + i + offset);
                        return expression_from_tokens(enclosed_tokens);
                    }

                    i += offset;
                }

                if (precedence > max_precedence) {
                    continue;
                }

                if (precedence == 4) {
                    if (tokens[i].type != TokenType::Integer ||
                        !tokens[i].value.has_value()) {
                        exit(1);
                    }
                    return std::unique_ptr<SyntaxNodeExpression>(
                        new SyntaxNodeExpression{ SyntaxNodeExpressionInteger{
                            tokens[i].value.value() } });
                }

                std::vector<Token> left_tokens(tokens.begin(),
                                               tokens.begin() + i);
                std::vector<Token> right_tokens(tokens.begin() + i + 1,
                                                tokens.end());

                return std::unique_ptr<SyntaxNodeExpression>(
                    new SyntaxNodeExpression{ SyntaxNodeExpressionArithmetic{
                        expression_from_tokens(left_tokens),
                        expression_from_tokens(right_tokens),
                        tokens[i].type } });
            }
            max_precedence++;
        }
    }

    void parse() {
        while (_tokens.size() > _offset) {
            auto token = consume();

            if (!token.has_value()) {
                break;
            }

            switch (token.value().type) {
                case TokenType::Let: {
                    auto identifier = consume();
                    if (!identifier.has_value() ||
                        identifier.value().type != TokenType::Identifier ||
                        !identifier.value().value.has_value()) {
                        exit(1);
                    }

                    auto operator_ = consume();
                    if (!operator_.has_value() ||
                        operator_.value().type != TokenType::Assign) {
                        exit(1);
                    }

                    std::vector<Token> expression_tokens;

                    while (peek().has_value() &&
                           peek().value().type != TokenType::Semicolon) {
                        expression_tokens.push_back(consume().value());
                    }

                    global_body.content.push_back(
                        SyntaxNodeStatement{ SyntaxNodeStatementLet{
                            identifier.value().value.value(),
                            expression_from_tokens(expression_tokens) } });
                    break;
                }
                default:
                    break;
            }
        }
    }

    std::optional<Token> peek(int offset = 0) {
        if (_tokens.size() >= _offset + offset) {
            return _tokens[_offset + offset];
        }

        return {};
    }

    std::optional<Token> consume() {
        if (_tokens.size() > _offset) {
            return _tokens[_offset++];
        }

        return {};
    }
};
