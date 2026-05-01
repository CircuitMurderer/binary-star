#include <format>
#include <stdexcept>
#include "spdlog/spdlog.h"
#include "ast_node.hpp"

namespace tidal::engine {
    using std::format;

    VariableNode::VariableNode(string n): name(std::move(n)) {
        this->node_count = 0;
    }

    FPArray VariableNode::eval(const Context& ctx, Calculator& calculator) {
        if (auto it = ctx.find(this->name); it != ctx.end()) 
            return it->second;
        spdlog::error("[ast_node] variable not found in context: {}", this->name);
        throw std::runtime_error(format("variable not found in context: {}", this->name));
    }

    BinaryOperationNode::BinaryOperationNode(unique_ptr<ASTNode> l, unique_ptr<ASTNode> r, string fn)
        : left(std::move(l)), right(std::move(r)), func_name(std::move(fn)) {
        this->node_count = 1 + this->left->size() + this->right->size();
    }

    FPArray BinaryOperationNode::eval(const Context& ctx, Calculator& calculator) {
        auto left_value = this->left->eval(ctx, calculator);
        auto right_value = this->right->eval(ctx, calculator);
        return calculator.calculate(this->func_name, left_value, right_value);
    }

    UnaryOperationNode::UnaryOperationNode(unique_ptr<ASTNode> c, string fn)
        : child(std::move(c)), func_name(std::move(fn)) {
        this->node_count = 1 + this->child->size();
    }

    FPArray UnaryOperationNode::eval(const Context& ctx, Calculator& calculator) {
        auto child_value = this->child->eval(ctx, calculator);
        return calculator.calculate(this->func_name, child_value);
    }
}