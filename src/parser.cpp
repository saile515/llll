#include "parser.hpp"

#include "lexer.hpp"

Parser::Parser(std::vector<Token> tokens)
  : _tokens(tokens) {
    parse();
}

static SyntaxNodeExpression expression_from_tokens(std::vector<Token> tokens) {
    for (int i = 0; i < tokens.size(); i++) {
        switch (tokens[i].type) {
            case TokenType::Add:
            case TokenType::Subtract: {
                std::vector<Token> left(tokens.begin(), tokens.begin() + i);
                std::vector<Token> right(tokens.begin() + i + 1, tokens.end());
                return SyntaxNodeExpressionArithmetic{
                    {},
                    expression_from_tokens(left),
                    expression_from_tokens(right),
                    tokens[i].type
                };
            }
            default:
                break;
        }
        if (tokens[i].type == TokenType::OpenParentheses) {
            int offset = 1;
            int open = 0; // Start at 0 since check happens before increment of
                          // last closing parentheses.
            int close = 0;

            while (tokens[i + offset].type != TokenType::CloseParentheses ||
                   open == close) {
                if (tokens[i + offset].type == TokenType::OpenParentheses) {
                    open++;
                } else if (tokens[i + offset].type ==
                           TokenType::CloseParentheses) {
                    close++;
                }
                i++;
                offset++;
            }
        }
    }
}

void Parser::parse() {
    while (_tokens.size() > _offset) {
        auto token = consume();

        if (!token.has_value()) {
            break;
        }

        switch (token.value().type) {
            case TokenType::Let:
                _global_body.content.push_back(
                    SyntaxNodeLet{ token.value().value });
                break;
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

mov rax, 1 add rax, 2
