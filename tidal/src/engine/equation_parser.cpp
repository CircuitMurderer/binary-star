#include <cctype>
#include <format>
#include <utility>
#include "spdlog/spdlog.h"
#include "types.hpp"
#include "equation_parser.hpp"

namespace tidal::engine {
    using std::unique_ptr;
    using std::make_unique;
    using std::isalpha;
    using std::isspace;
    using std::format;

    EquationParser::EquationParser() : pos(0), equation("") {}

    char EquationParser::peek() {
        while (pos < equation.size() && isspace(equation[pos])) pos++;
        if (pos == equation.size()) return 0;
        return equation[pos];
    }

    char EquationParser::get() {
        char c = peek();
        if (pos < equation.size()) pos++;
        return c;
    }

    unique_ptr<ASTNode> EquationParser::parse(const string& expression) {
        this->equation = expression;
        this->pos = 0;

        return this->parse_expression();
    }

    unique_ptr<ASTNode> EquationParser::parse_expression() {
        auto left = this->parse_term();

        while (true) {
            auto c = this->peek();
            if (c == ADD) {
                this->get();
                auto right = this->parse_term();
                left = make_unique<BinaryOperationNode>(std::move(left), std::move(right), "add");

            } else if (c == SUB) {
                this->get();
                auto right = this->parse_term();
                left = make_unique<BinaryOperationNode>(std::move(left), std::move(right), "sub");

            } else {
                break;
            }
        }

        return left;
    }

    unique_ptr<ASTNode> EquationParser::parse_term() {
        auto left = this->parse_factor();

        while (true) {
            auto c = this->peek();
            if (c == MUL) {
                this->get();
                auto right = this->parse_factor();
                left = make_unique<BinaryOperationNode>(std::move(left), std::move(right), "mul");

            } else if (c == DIV) {
                this->get();
                auto right = this->parse_factor();
                left = make_unique<BinaryOperationNode>(std::move(left), std::move(right), "div");

            } else {
                break;
            }
        }

        return left;
    } 

    unique_ptr<ASTNode> EquationParser::parse_factor() {
        auto c = this->peek();

        if (c == L_PAR) {
            this->get();
            auto node = this->parse_expression();
            if (this->get() != R_PAR) {
                spdlog::error("[equation_parser] expected ')'");
                throw std::runtime_error("expected ')'");
            }
            
            return node;
        }

        if (isalpha(c)) {
            string name;
            while (std::isalnum(this->peek()) || this->peek() == '_') 
                name += this->get();
            
            if (this->peek() == L_PAR) {
                this->get();

                if (BINARY_FUNC_NAMES.count(name)) {
                    auto arg1 = this->parse_expression();
                    if (this->get() != COMMA) {
                        spdlog::error("[equation_parser] expected ',' in binary function {}", name);
                        throw std::runtime_error(format("expected ',' in binary function {}", name));
                    }

                    auto arg2 = this->parse_expression();
                    if (this->get() != R_PAR) {
                        spdlog::error("[equation_parser] expected ')' after function args");
                        throw std::runtime_error("expected ')' after function args");
                    }

                    return make_unique<BinaryOperationNode>(std::move(arg1), std::move(arg2), name);
                } else if (UNARY_FUNC_NAMES.count(name)) {
                    auto arg = this->parse_expression();
                    if (this->get() != R_PAR) {
                        spdlog::error("[equation_parser] expected ')' after function arg");
                        throw std::runtime_error("expected ')' after function arg");
                    }

                    return make_unique<UnaryOperationNode>(std::move(arg), name);
                } else {
                    spdlog::error("[equation_parser] unknown function: {}", name);
                    throw std::runtime_error(format("unknown function: {}", name));
                }
            }

            return make_unique<VariableNode>(name);
        }

        spdlog::error("[equation_parser] unexpected character: {}", c);
        throw std::runtime_error(format("unexpected character: {}", c));
    }
}