/**
 * Project: BIN project
 * Author: Tomáš Kučma
 * Year: 2022/2023
 * Description: Contains definition of types and declaration of functions used
 *  for function blocks [standard, not XMG, variant]
 */

#ifndef STANDARD_FUNCTION_HPP
#define STANDARD_FUNCTION_HPP

#include "types.hpp"
#include <array>
#include <stdexcept>
#include <string>

constexpr size_t BLOCK_IN_COUNT = 2;
constexpr size_t COST = 1;
constexpr size_t MAX_BLOCK_COST = COST;

using BlockInput = std::array<Bitmap, BLOCK_IN_COUNT>;

enum Function {
    AND,
    OR,
    XOR,
    NAND,
    NOR,
    NXOR,
    FUNCTION_COUNT,
};

class FunctionError : public std::invalid_argument {
  public:
    FunctionError(const Function &function)
        : std::invalid_argument(std::string{"Invalid function block "} +
                                std::to_string(function)){};
};

constexpr size_t function_in_count(const Function &function) { return 2; }

// returns cost assigned to a given function block
constexpr size_t function_cost(const Function &function) { return COST; }

constexpr Bitmap simulate_function(const BlockInput &inputs,
                                   const Function &function) {
    switch (function) {
    case AND:
        return inputs[0] & inputs[1];
    case OR:
        return inputs[0] | inputs[1];
    case XOR:
        return inputs[0] ^ inputs[1];
    case NAND:
        return ~(inputs[0] & inputs[1]);
    case NOR:
        return ~(inputs[0] | inputs[1]);
    case NXOR:
        return ~(inputs[0] ^ inputs[1]);
    default:
        throw FunctionError(function);
    }
}

#endif // STANDARD_FUNCTION_HPP
