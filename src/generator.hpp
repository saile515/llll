#pragma once

#include "parser.hpp"
#include <iostream>
#include <sstream>
#include <variant>

struct Variable {
    std::string identifier;
    unsigned int stack_location;
};

class Generator {
private:
    SyntaxNodeBody& _root_node;
    std::vector<int> _scopes;

public:
    std::vector<Variable> variables;
    std::stringstream output;
    unsigned int stack_size = 0;

    Generator(SyntaxNodeBody& root_node)
      : _root_node(root_node) {
        generate();
    };

    void generate_expression(SyntaxNodeExpression& node) {
        struct ExpressionVisit {
            Generator& generator;

            ExpressionVisit(Generator& generator)
              : generator(generator){};

            void operator()(SyntaxNodeExpressionInteger& node) {
                generator.output << "    mov rax, " << node.value << "\n";
                generator.push("rax");
            }

            void operator()(SyntaxNodeExpressionCompare& node){};

            void operator()(SyntaxNodeExpressionArithmetic& node) {
                generator.generate_expression(*node.left);
                generator.generate_expression(*node.right);

                switch (node.operator_) {
                    case TokenType::Add:
                        generator.pop("rax");
                        generator.pop("rbx");
                        generator.output << "    add rax, rbx\n";
                        generator.push("rax");
                        break;
                    case TokenType::Subtract:
                        generator.pop("rbx");
                        generator.pop("rax");
                        generator.output << "    sub rax, rbx\n";
                        generator.push("rax");
                        break;
                    case TokenType::Multiply:
                        generator.output << "    mov rdx, 0\n";
                        generator.pop("rax");
                        generator.pop("rcx");
                        generator.output << "    mul rcx\n";
                        generator.push("rax");
                        break;
                    case TokenType::Divide:
                        generator.output << "    mov rdx, 0\n";
                        generator.pop("rcx");
                        generator.pop("rax");
                        generator.output << "    div rcx\n";
                        generator.push("rax");
                    default:
                        exit(1);
                }
            };
        };

        ExpressionVisit expression_visit(*this);

        std::visit(expression_visit, node);
    }

    void push(std::string register_) {
        stack_size += 8;
        output << "    push " << register_ << "\n";
    };

    void pop(std::string register_) {
        stack_size -= 8;
        output << "    pop " << register_ << "\n";
    }

private:
    void generate() {
        output << "global _start\n"
               << "section .text\n"
               << "_start:\n";

        begin_scope();

        generate_body(_root_node);

        output << "    mov rax, 60\n"
               << "    pop rdi\n"
               << "    syscall\n";
    };

    void generate_body(SyntaxNodeBody& node) {
        for (auto& child_node : node.content) {
            generate_statment(child_node);
        }
    };

    void generate_statment(SyntaxNodeStatement& node) {
        struct StatementVisit {
            Generator& generator;

            StatementVisit(Generator& generator)
              : generator(generator){};
            ;

            void operator()(SyntaxNodeStatementLet& node) {
                generator.generate_expression(*node.value);
                generator.variables.push_back(
                    Variable{ node.identifier, generator.stack_size });
            }
        };

        StatementVisit statement_visit(*this);

        std::visit(statement_visit, node);
    }

    void begin_scope() { _scopes.push_back(0); }

    void end_scope() {
        output << "    add rsp, " << _scopes.back() * 8;
        _scopes.pop_back();
    }
};
