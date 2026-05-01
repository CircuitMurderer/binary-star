#pragma once
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <utils/constants.h>
#include <vector>
#include "FloatingPoint/floating-point.h"
#include "FloatingPoint/fp-math.h"
#include "types.hpp"

namespace tidal::engine {
    using std::vector;
    using std::string;
    using std::string_view;
    using std::unordered_map;
    using std::unique_ptr;
    using std::equal_to;
    using std::function;
    using std::pair;

    using sci::IOPack;
    using sci::OTPack;

    struct StringHash {
        using is_transparent = void;
        size_t operator()(string_view sv) const;
        size_t operator()(const string& s) const;
    };

    class Calculator {
    public:
        explicit Calculator(int role, int port, string address);
        ~Calculator() = default;

        Calculator(const Calculator&) = delete;
        Calculator& operator=(const Calculator&) = delete;
        Calculator(Calculator&&) = delete;      
        Calculator& operator=(Calculator&&) = delete;

        FPArray input(vector<Number>& plain, bool dummy = false);
        vector<Number> output(FPArray& cypher);

        FPArray calculate(string_view func_name, const FPArray& operand);
        FPArray calculate(string_view func_name, const FPArray& operand_left, const FPArray& operand_right);
    
        void init_progress(size_t total);
        pair<size_t, size_t> tick_tock(bool verbose = false);

    private:
        void register_operations();

        unordered_map<string, UnaryFunc, StringHash, equal_to<>> unary_funcs;
        unordered_map<string, BinaryFunc, StringHash, equal_to<>> binary_funcs;

        int cal_role;
        unique_ptr<IOPack> io_pack;
        unique_ptr<OTPack> ot_pack;
        unique_ptr<FPOp> fp_op;
        unique_ptr<FPMath> fp_math;

        size_t total_steps{0};
        size_t current_step{0};
    };
}
    