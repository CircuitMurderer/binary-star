#pragma once
#include <memory>
#include <string>
#include <unordered_set>
#include "ast_node.hpp"

namespace tidal::engine {
    using std::string;
    using std::unique_ptr;
    using std::unordered_set;

    class EquationParser {
    public:
        explicit EquationParser();
        ~EquationParser() = default;

        unique_ptr<ASTNode> parse(const string& expression);
        
    private:
        string equation;
        size_t pos;

        char peek();
        char get();
        bool matchString(const string& s);

        unique_ptr<ASTNode> parseExpression();  // +, -
        unique_ptr<ASTNode> parseTerm();        // *, /
        unique_ptr<ASTNode> parseFactor();      // (), func, var
    };
}