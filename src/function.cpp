/**
 * Project: BIN project
 * Author: Tomáš Kučma
 * Year: 2022/2023
 * Description: Contains implementation of functions used for function blocks
 */

#include "function.hpp"

bool is_xor(const Function &function) {
    return XOR_00 <= function && function <= XOR_11;
}

bool is_maj(const Function &function) {
    return MAJ_000 <= function && function <= MAJ_111;
}

size_t function_in_count(const Function &function) {
    if (is_xor(function)) {
        return 2;
    } else if (is_maj(function)) {
        return 3;
    } else {
        throw FunctionError(function);
    }
}

size_t function_cost(const Function &function) {
    if (is_xor(function)) {
        return XOR_COST;
    } else if (is_maj(function)) {
        return MAJ_COST;
    } else {
        throw FunctionError(function);
    }
}

Bitmap simulate_function(const Bitmap &x, const Bitmap &y, const Bitmap &z,
                         const Function &function) {
    switch (function) {
    case XOR_00:
        return ~x ^ ~y;
    case XOR_01:
        return ~x ^ y;
    case XOR_10:
        return x ^ ~y;
    case XOR_11:
        return x ^ y;
    case MAJ_000:
        return (~x & ~y) ^ (~x & ~z) ^ (~y & ~z);
    case MAJ_001:
        return (~x & ~y) ^ (~x & z) ^ (~y & z);
    case MAJ_010:
        return (~x & y) ^ (~x & ~z) ^ (y & ~z);
    case MAJ_011:
        return (~x & y) ^ (~x & z) ^ (y & z);
    case MAJ_100:
        return (x & ~y) ^ (x & ~z) ^ (~y & ~z);
    case MAJ_101:
        return (x & ~y) ^ (x & z) ^ (~y & z);
    case MAJ_110:
        return (x & y) ^ (x & ~z) ^ (y & ~z);
    case MAJ_111:
        return (x & y) ^ (x & z) ^ (y & z);
    default:
        throw FunctionError(function);
    }
}
