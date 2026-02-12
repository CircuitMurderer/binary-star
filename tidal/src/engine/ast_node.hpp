#pragma once
#include <memory>
#include <string>
#include "calculator.hpp"

namespace tidal::engine {
    using std::string;
    using std::unordered_map;
    using std::unique_ptr;

    class ASTNode {
    public:
        virtual ~ASTNode() = default;
        virtual FPArray eval(const Context& ctx, Calculator& calculator) = 0;
    };

    class VariableNode : public ASTNode {
    public:
        explicit VariableNode(string n);
        FPArray eval(const Context& ctx, Calculator& calculator) override;
    private:
        string name;
    };

    class BinaryOperationNode : public ASTNode {
    public:
        explicit BinaryOperationNode(unique_ptr<ASTNode> l, unique_ptr<ASTNode> r, string fn);
        FPArray eval(const Context& ctx, Calculator& calculator) override;
    private:
        unique_ptr<ASTNode> left;
        unique_ptr<ASTNode> right;
        string funcName;
    };

    class UnaryOperationNode : public ASTNode {
    public:
        explicit UnaryOperationNode(unique_ptr<ASTNode> c, string fn);
        FPArray eval(const Context& ctx, Calculator& calculator) override;
    private:
        unique_ptr<ASTNode> child;
        string funcName;
    };
}