#pragma once
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include "utils/constants.h"
#include "FloatingPoint/floating-point.h"

namespace tidal::engine {
    using std::function;
    using std::unordered_map;
    using std::unordered_set;

    using Number = float;
    using Context = unordered_map<string, FPArray>;
    using UnaryFunc = function<FPArray(const FPArray&)>;
    using BinaryFunc = function<FPArray(const FPArray&, const FPArray&)>;

    constexpr int PUBLIC = sci::PUBLIC;
    constexpr int ALICE = sci::ALICE;
    constexpr int BOB = sci::BOB;
    constexpr int OPPOSITE = ALICE + BOB;

    constexpr char ADD = '+';
    constexpr char SUB = '-';
    constexpr char MUL = '*';
    constexpr char DIV = '/';
    constexpr char L_PAR = '(';
    constexpr char R_PAR = ')';
    constexpr char COMMA = ',';

    const unordered_set<string> UnaryFuncNames = {"sinpi", "cospi", "tanpi", "exp2", "log2", "exp", "ln", "erf"};
    const unordered_set<string> BinaryFuncNames = {"add", "sub", "mul", "div", "max", "min"};
}