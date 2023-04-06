#include "function.hpp"
#include <stdexcept>
#include <string>

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
        throw std::invalid_argument(std::string{"Invalid function block "} +
                                    std::to_string(function));
    }
}