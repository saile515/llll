#include "parser.hpp"

#include "lexer.hpp"
#include <iostream>

Parser::Parser(std::vector<Token> tokens)
  : _tokens(tokens) {
    parse();
}

static SyntaxNodeExpression expression_from_tokens(std::vector<Token> tokens) {
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
                int open = 0; // Start at 0 since check happens before increment
                              // of last closing parentheses.
                int close = 0;

                while (tokens[i + offset].type != TokenType::CloseParentheses ||
                       open == close) {
                    if (tokens[i + offset].type == TokenType::OpenParentheses) {
                        open++;
                    } else if (tokens[i + offset].type ==
                               TokenType::CloseParentheses) {
                        close++;
                    }
                    offset++;
                }

                if (max_precedence == precedence) {
                    std::vector<Token> enclosed_tokens(tokens.begin() + i + 1,
                                                       tokens.begin() + i +
                                                           offset - 1);
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
                return SyntaxNodeExpressionInteger{ {},
                                                    tokens[i].value.value() };
            }

            std::vector<Token> left(tokens.begin(), tokens.begin() + i);
            std::vector<Token> right(tokens.begin() + i + 1, tokens.end());

            return SyntaxNodeExpressionArithmetic{ {},
                                                   expression_from_tokens(left),
                                                   expression_from_tokens(
                                                       right),
                                                   tokens[i].type };
        }
        max_precedence++;
    }
}

void Parser::parse() {
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

                _global_body.content.push_back(
                    SyntaxNodeLet{ {},
                                   identifier.value().value.value(),
                                   expression_from_tokens(expression_tokens) });
                break;
            }
            default:
                break;
        }
    }
}

std::optional<Token> Parser::peek(int offset) {
    if (_tokens.size() >= _offset + offset) {
        return _tokens[_offset + offset];
    }

    return {};
}

std::optional<Token> Parser::consume() {
    if (_tokens.size() > _offset) {
        return _tokens[_offset++];
    }

    return {};
}
