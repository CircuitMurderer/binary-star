#include <cctype>
#include <utility>
#include "types.hpp"
#include "equation_parser.hpp"

namespace tidal::engine {
    using std::unique_ptr;
    using std::make_unique;
    using std::isalpha;
    using std::isspace;

    EquationParser::EquationParser() : pos(0), equation("") {}

    char EquationParser::peek() {
        while (pos < equation.size() && std::isspace(equation[pos])) pos++;
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

        return this->parseExpression();
    }

    unique_ptr<ASTNode> EquationParser::parseExpression() {
        auto left = this->parseTerm();

        while (true) {
            auto c = this->peek();
            if (c == ADD) {
                this->get();
                auto right = this->parseTerm();
                left = make_unique<BinaryOperationNode>(std::move(left), std::move(right), "add");

            } else if (c == SUB) {
                this->get();
                auto right = this->parseTerm();
                left = make_unique<BinaryOperationNode>(std::move(left), std::move(right), "sub");

            } else {
                break;
            }
        }

        return left;
    }

    unique_ptr<ASTNode> EquationParser::parseTerm() {
        auto left = this->parseFactor();

        while (true) {
            auto c = this->peek();
            if (c == MUL) {
                this->get();
                auto right = this->parseFactor();
                left = make_unique<BinaryOperationNode>(std::move(left), std::move(right), "mul");

            } else if (c == DIV) {
                this->get();
                auto right = this->parseFactor();
                left = make_unique<BinaryOperationNode>(std::move(left), std::move(right), "div");

            } else {
                break;
            }
        }

        return left;
    } 

    unique_ptr<ASTNode> EquationParser::parseFactor() {
        auto c = this->peek();

        if (c == L_PAR) {
            this->get();
            auto node = this->parseExpression();
            if (this->get() != R_PAR) throw std::runtime_error("expected ')'");
            
            return node;
        }

        if (isalpha(c)) {
            string name;
            while (std::isalnum(this->peek()) || this->peek() == '_') 
                name += this->get();
            
            if (this->peek() == L_PAR) {
                this->get();

                if (BinaryFuncNames.count(name)) {
                    auto arg1 = this->parseExpression();
                    if (this->get() != COMMA) 
                        throw std::runtime_error("Expected ',' in binary function " + name);

                    auto arg2 = this->parseExpression();
                    if (this->get() != R_PAR) 
                        throw std::runtime_error("Expected ')' after function args");

                    return make_unique<BinaryOperationNode>(std::move(arg1), std::move(arg2), name);
                } else if (UnaryFuncNames.count(name)) {
                    auto arg = this->parseExpression();
                    if (this->get() != R_PAR) 
                        throw std::runtime_error("Expected ')' after function arg");

                    return make_unique<UnaryOperationNode>(std::move(arg), name);
                } else {
                    throw std::runtime_error("Unknown function: " + name);
                }
            }

            return make_unique<VariableNode>(name);
        }

        throw std::runtime_error(std::string("Unexpected character: ") + c);
    }
}