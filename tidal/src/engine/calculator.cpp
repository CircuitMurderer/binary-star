#include <format>
#include <stdexcept>
#include <utility>
#include "spdlog/spdlog.h"
#include "calculator.hpp"
#include "types.hpp"

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
        this->io_pack = make_unique<IOPack>(role, port, address);
        this->ot_pack = make_unique<OTPack>(this->io_pack.get(), role);

        this->fp_op = make_unique<FPOp>(role, this->io_pack.get(), this->ot_pack.get());
        this->fp_math = make_unique<FPMath>(role, this->io_pack.get(), this->ot_pack.get());
        
        this->cal_role = role;
        register_operations();
    }

    FPArray Calculator::input(vector<Number>& plain, bool dummy) {
        int input_party = dummy ? (this->cal_role | OPPOSITE) : this->cal_role;
        return this->fp_op->input(input_party, plain.size(), plain.data());
    }

    vector<Number> Calculator::output(FPArray& cypher) {
        return this->fp_op->output(PUBLIC, cypher).get_native_type<Number>();
    }

    FPArray Calculator::calculate(string_view func_name, const FPArray& operand) {
        if (auto it = this->unary_funcs.find(func_name); it != unary_funcs.end()) {
            auto res = it->second(operand);
            this->tick_tock();
            return res;
        }
            
        spdlog::error("[calculator] unknown unary function: {}", func_name);
        throw std::runtime_error(format("unknown unary function: {}", func_name));
    }

    FPArray Calculator::calculate(string_view func_name, const FPArray& operand_left, const FPArray& operand_right) {
        if (auto it = this->binary_funcs.find(func_name); it != binary_funcs.end()) {
            auto res = it->second(operand_left, operand_right);
            this->tick_tock();
            return res;
        }

        spdlog::error("[calculator] unknown binary function: {}", func_name);
        throw std::runtime_error(format("unknown binary function: {}", func_name));
    }

    void Calculator::register_operations() {
        this->unary_funcs = {
            {"sinpi", [this](const FPArray& x) { return this->fp_math->sinpi(x); }},
            {"cospi", [this](const FPArray& x) { return this->fp_math->cospi(x); }},
            {"tanpi", [this](const FPArray& x) { return this->fp_math->tanpi(x); }},
            {"exp2", [this](const FPArray& x) { return this->fp_math->exp2(x); }},
            {"log2", [this](const FPArray& x) { return this->fp_math->log2(x); }},
            {"exp", [this](const FPArray& x) { return this->fp_math->exp(x); }},
            {"ln", [this](const FPArray& x) { return this->fp_math->ln(x); }},
            {"erf", [this](const FPArray& x) { return this->fp_math->erf(x); }},
        };

        this->binary_funcs = {
            {"add", [this](const FPArray& x, const FPArray& y) { return this->fp_op->add(x, y); }},
            {"sub", [this](const FPArray& x, const FPArray& y) { return this->fp_op->sub(x, y); }},
            {"mul", [this](const FPArray& x, const FPArray& y) { return this->fp_op->mul(x, y); }},
            {"div", [this](const FPArray& x, const FPArray& y) { return this->fp_op->div(x, y); }},
            {"max", [this](const FPArray& x, const FPArray& y) { return this->fp_op->max(x, y); }},
            {"min", [this](const FPArray& x, const FPArray& y) { return this->fp_op->min(x, y); }},
        };
    }

    void Calculator::init_progress(size_t total) {
        this->total_steps = total;
        this->current_step = 0;
    }

    pair<size_t, size_t> Calculator::tick_tock(bool verbose) {
        this->current_step++;
        if (verbose)
            spdlog::info("[calculator] progress: {}/{}", this->current_step, this->total_steps);
        return std::make_pair(this->current_step, this->total_steps);
    }
}