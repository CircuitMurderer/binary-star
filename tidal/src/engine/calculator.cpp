#include <format>
#include <stdexcept>
#include "calculator.hpp"

namespace tidal::engine {
    using std::make_unique;
    using std::format;
    using std::hash;

    size_t StringHash::operator()(string_view sv) const {
        return hash<string_view>{}(sv);
    }

    size_t StringHash::operator()(const string& s) const {
        return hash<string_view>{}(s);
    }

    Calculator::Calculator(int role, int port, string address) {
        this->ioPack = make_unique<IOPack>(role, port, address);
        this->otPack = make_unique<OTPack>(this->ioPack.get(), role);

        this->fpOp = make_unique<FPOp>(role, this->ioPack.get(), this->otPack.get());
        this->fpMath = make_unique<FPMath>(role, this->ioPack.get(), this->otPack.get());
        
        this->calRole = role;
        registerOperations();
    }

    FPArray Calculator::input(vector<Number>& plain, bool dummy) {
        int inputParty = dummy ? (OPPOSITE - this->calRole) : this->calRole;
        return this->fpOp->input(inputParty, plain.size(), plain.data());
    }

    vector<Number> Calculator::output(FPArray& cypher) {
        return this->fpOp->output(PUBLIC, cypher).get_native_type<Number>();
    }

    FPArray Calculator::calculate(string_view funcName, const FPArray& operand) {
        if (auto it = this->unaryFuncMap.find(funcName); it != unaryFuncMap.end())
            return it->second(operand);
        throw std::runtime_error(format("Unknown unary function: {}", funcName));
    }

    FPArray Calculator::calculate(string_view funcName, const FPArray& operandLeft, const FPArray& operandRight) {
        if (auto it = this->binaryFuncMap.find(funcName); it != binaryFuncMap.end())
            return it->second(operandLeft, operandRight);
        throw std::runtime_error(format("Unknown binary function: {}", funcName));
    }

    void Calculator::registerOperations() {
        this->unaryFuncMap = {
            {"sinpi", [this](const FPArray& x) { return this->fpMath->sinpi(x); }},
            {"cospi", [this](const FPArray& x) { return this->fpMath->cospi(x); }},
            {"tanpi", [this](const FPArray& x) { return this->fpMath->tanpi(x); }},
            {"exp2", [this](const FPArray& x) { return this->fpMath->exp2(x); }},
            {"log2", [this](const FPArray& x) { return this->fpMath->log2(x); }},
            {"exp", [this](const FPArray& x) { return this->fpMath->exp(x); }},
            {"ln", [this](const FPArray& x) { return this->fpMath->ln(x); }},
            {"erf", [this](const FPArray& x) { return this->fpMath->erf(x); }},
        };

        this->binaryFuncMap = {
            {"add", [this](const FPArray& x, const FPArray& y) { return this->fpOp->add(x, y); }},
            {"sub", [this](const FPArray& x, const FPArray& y) { return this->fpOp->sub(x, y); }},
            {"mul", [this](const FPArray& x, const FPArray& y) { return this->fpOp->mul(x, y); }},
            {"div", [this](const FPArray& x, const FPArray& y) { return this->fpOp->div(x, y); }},
            {"max", [this](const FPArray& x, const FPArray& y) { return this->fpOp->max(x, y); }},
            {"min", [this](const FPArray& x, const FPArray& y) { return this->fpOp->min(x, y); }},
        };
    }
}