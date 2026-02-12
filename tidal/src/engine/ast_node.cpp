#include <format>
#include <stdexcept>
#include "ast_node.hpp"

namespace tidal::engine {
    using std::format;

    VariableNode::VariableNode(string n): name(std::move(n)) {}

    FPArray VariableNode::eval(const Context& ctx, Calculator& calculator) {
        if (auto it = ctx.find(this->name); it != ctx.end()) 
            return it->second;
        throw std::runtime_error(format("variable not found in context: {}", this->name));
    }

    BinaryOperationNode::BinaryOperationNode(unique_ptr<ASTNode> l, unique_ptr<ASTNode> r, string fn)
        : left(std::move(l)), right(std::move(r)), funcName(std::move(fn)) {}

    FPArray BinaryOperationNode::eval(const Context& ctx, Calculator& calculator) {
        auto leftValue = this->left->eval(ctx, calculator);
        auto rightValue = this->right->eval(ctx, calculator);
        return calculator.calculate(this->funcName, leftValue, rightValue);
    }

    UnaryOperationNode::UnaryOperationNode(unique_ptr<ASTNode> c, string fn)
        : child(std::move(c)), funcName(std::move(fn)) {}

    FPArray UnaryOperationNode::eval(const Context& ctx, Calculator& calculator) {
        auto childValue = this->child->eval(ctx, calculator);
        return calculator.calculate(this->funcName, childValue);
    }
}