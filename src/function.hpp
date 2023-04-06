#include "types.hpp"

#ifndef FUNCTION_HPP
#define FUNCTION_HPP

constexpr size_t BLOCK_IN_COUNT = 3;

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

Bitmap simulate_function(const Bitmap &x, const Bitmap &y, const Bitmap &z,
                         const Function &function);

#endif // FUNCTION_HPP
