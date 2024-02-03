#pragma once

#include <optional>
#include <string>
#include <unordered_map>
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

static std::unordered_map<std::string, TokenType> keyword_to_token_map{
    { "let", TokenType::Let }
};

class Lexer {
private:
    std::string _input;
    unsigned int _offset = 0;

public:
    std::vector<Token> tokens;

    Lexer(std::string input)
      : _input(input) {
        lex();
    }

private:
    void lex() {
        while (_offset < _input.length()) {
            std::optional<char> character = consume();

            if (!character.has_value()) {
                continue;
            }

            if (std::isalpha(character.value()) ||
                character.value() == '_') { // Identifier/keyword
                std::string statement(1, character.value());
                while (true) {
                    auto next_char = peek();

                    if (next_char.has_value() &&
                        (std::isalnum(next_char.value()) ||
                         next_char.value() == '_')) {
                        statement += next_char.value();
                        consume();
                    } else {
                        break;
                    }
                }

                if (keyword_to_token_map.count(statement)) {
                    auto keyword = keyword_to_token_map.at(statement);
                    tokens.push_back({ keyword });
                } else {
                    tokens.push_back({ TokenType::Identifier, statement });
                }
            } else if (character.value() == '=') { // Assign/Equals
                if (peek().has_value() && peek().value() != '=') {
                    tokens.push_back({ TokenType::Assign });
                } else {
                    tokens.push_back({ TokenType::Equals });
                    consume();
                }
            } else if (character.value() == '+') { // AddAssign/Increment/Add
                if (peek().has_value() && peek().value() == '=') {
                    tokens.push_back({ TokenType::AddAssign });
                    consume();
                } else if (peek().has_value() && peek().value() == '+') {
                    tokens.push_back({ TokenType::Increment });
                    consume();
                } else {
                    tokens.push_back({ TokenType::Add });
                }
            } else if (character.value() ==
                       '-') { // SubtractAssign/Decrement/Subtract
                if (peek().has_value() && peek().value() == '=') {
                    tokens.push_back({ TokenType::SubtractAssign });
                    consume();
                } else if (peek().has_value() && peek().value() == '-') {
                    tokens.push_back({ TokenType::Decrement });
                    consume();
                } else {
                    tokens.push_back({ TokenType::Subtract });
                }
            } else if (character.value() ==
                       '*') { // MultiplyAssign/Power/Multiply
                if (peek().has_value() && peek().value() == '=') {
                    tokens.push_back({ TokenType::MultiplyAssign });
                    consume();
                } else if (peek().has_value() && peek().value() == '*') {
                    tokens.push_back({ TokenType::Power });
                    consume();
                } else {
                    tokens.push_back({ TokenType::Multiply });
                }
            } else if (character.value() ==
                       '/') { // DivideAssign/comments/Divide
                if (peek().has_value() && peek().value() == '=') {
                    tokens.push_back({ TokenType::DivideAssign });
                    consume();
                } else if (peek().has_value() && peek().value() == '*') {
                    while (true) {
                        auto next_char = consume();
                        if (next_char.has_value() && next_char.value() == '*' &&
                            peek().has_value() && peek().value() == '/') {
                            consume();
                            break;
                        }
                    }
                } else if (peek().has_value() && peek().value() == '/') {
                    while (true) {
                        auto next_char = consume();
                        if (next_char.has_value() &&
                            next_char.value() == '\n') {
                            break;
                        }
                    }
                } else {
                    tokens.push_back({ TokenType::Divide });
                }
            } else if (std::isdigit(character.value())) { // Integer
                std::string integer(1, character.value());
                while (true) {
                    auto next_char = peek();

                    if (next_char.has_value() &&
                        std::isdigit(next_char.value())) {
                        integer += next_char.value();
                        consume();
                    } else {
                        break;
                    }
                }
                tokens.push_back({ TokenType::Integer, integer });
            } else if (character.value() == ';') {
                tokens.push_back({ TokenType::Semicolon });
            } else if (character.value() == '(') {
                tokens.push_back({ TokenType::OpenParentheses });
            } else if (character.value() == ')') {
                tokens.push_back({ TokenType::CloseParentheses });
            }
        }
    }

    std::optional<char> peek(int offset = 0) {
        if (_input.length() >= _offset + offset) {
            return _input[_offset + offset];
        }

        return {};
    }

    std::optional<char> consume() {
        if (_input.length() > _offset) {
            return _input[_offset++];
        }

        return {};
    }
};
