/**
 * Project: BIN project
 * Author: Tomáš Kučma
 * Year: 2022/2023
 * Description: Contains definition of types and declaration of functions used
 *  for function blocks
 */

#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#ifdef STANDARD_VARIANT
#include "standard_function.hpp"
#else // STANDARD_VARIANT

#include "types.hpp"
#include <stdexcept>
#include <string>

constexpr size_t BLOCK_IN_COUNT = 3;
constexpr size_t XOR_COST = 1;
constexpr size_t MAJ_COST = 2;
constexpr size_t MAX_BLOCK_COST = std::max(XOR_COST, MAJ_COST);

enum Function {
    XOR_01,
    XOR_11,
    MAJ_000,
    MAJ_001,
    MAJ_011,
    MAJ_111,
    FUNCTION_COUNT,
};

class FunctionError : public std::invalid_argument {
  public:
    FunctionError(const Function &function)
        : std::invalid_argument(std::string{"Invalid function block "} +
                                std::to_string(function)){};
};

constexpr bool is_xor(const Function &function) {
    return XOR_01 <= function && function <= XOR_11;
}

constexpr bool is_maj(const Function &function) {
    return MAJ_000 <= function && function <= MAJ_111;
}

constexpr size_t function_in_count(const Function &function) {
    if (is_xor(function)) {
        return 2;
    } else if (is_maj(function)) {
        return 3;
    } else {
        throw FunctionError(function);
    }
}

// returns cost assigned to a given function block
constexpr size_t function_cost(const Function &function) {
    if (is_xor(function)) {
        return XOR_COST;
    } else if (is_maj(function)) {
        return MAJ_COST;
    } else {
        throw FunctionError(function);
    }
}

constexpr Bitmap simulate_function(const Bitmap &x, const Bitmap &y,
                                   const Bitmap &z, const Function &function) {
    switch (function) {
    case XOR_01:
        return ~x ^ y;
    case XOR_11:
        return x ^ y;
    case MAJ_000:
        return (~x & ~y) ^ (~x & ~z) ^ (~y & ~z);
    case MAJ_001:
        return (~x & ~y) ^ (~x & z) ^ (~y & z);
    case MAJ_011:
        return (~x & y) ^ (~x & z) ^ (y & z);
    case MAJ_111:
        return (x & y) ^ (x & z) ^ (y & z);
    default:
        throw FunctionError(function);
    }
}

#endif // STANDARD_VARIANT

#endif // FUNCTION_HPP
