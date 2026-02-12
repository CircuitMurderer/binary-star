#pragma once
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
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

        FPArray calculate(string_view funcName, const FPArray& operand);
        FPArray calculate(string_view funcName, const FPArray& operandLeft, const FPArray& operandRight);
    
    private:
        void registerOperations();

        unordered_map<string, UnaryFunc, StringHash, equal_to<>> unaryFuncMap;
        unordered_map<string, BinaryFunc, StringHash, equal_to<>> binaryFuncMap;

        int calRole;
        unique_ptr<IOPack> ioPack;
        unique_ptr<OTPack> otPack;
        unique_ptr<FPOp> fpOp;
        unique_ptr<FPMath> fpMath;
    };
}
    