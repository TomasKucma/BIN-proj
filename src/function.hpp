/**
 * Project: BIN project
 * Author: Tomáš Kučma
 * Year: 2022/2023
 * Description: Contains definition of types and declaration of functions used
 *  for function blocks
 */

#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include "types.hpp"
#include <stdexcept>
#include <string>

constexpr size_t BLOCK_IN_COUNT = 3;
constexpr size_t XOR_COST = 1;
constexpr size_t MAJ_COST = 2;
constexpr size_t MAX_BLOCK_COST = std::max(XOR_COST, MAJ_COST);

enum Function {
    XOR_00,
    XOR_01,
    XOR_10,
    XOR_11,
    MAJ_000,
    MAJ_001,
    MAJ_010,
    MAJ_011,
    MAJ_100,
    MAJ_101,
    MAJ_110,
    MAJ_111,
    FUNCTION_COUNT,
};

class FunctionError : public std::invalid_argument {
  public:
    FunctionError(const Function &function)
        : std::invalid_argument(std::string{"Invalid function block "} +
                                std::to_string(function)){};
};

size_t function_in_count(const Function &function);

// returns cost assigned to a given function block
size_t function_cost(const Function &function);

Bitmap simulate_function(const Bitmap &x, const Bitmap &y, const Bitmap &z,
                         const Function &function);

#endif // FUNCTION_HPP
